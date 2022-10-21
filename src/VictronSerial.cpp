/*
 * File: VictronSerial.cpp
 * Project: gardener
 * Created Date: Friday October 21st 2022
 * Author: Kyle Hofer
 * 
 * MIT License
 * 
 * Copyright (c) 2022 Kyle Hofer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * HISTORY:
 */

#include "VictronSerial.h"

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <stdint.h>

#define IDLE_TIME (CLOCKS_PER_SEC << 2)

// Convert a set of characters into a unique binary ID
#define BUILD_RAW_ID(...) BUILD_RAW_ID_ARG(__VA_ARGS__, 0, 0, 0, 0)
#define BUILD_RAW_ID_ARG(_1, _2, _3, _4, ...) ((_1) + ((_2) << 8) + ((_3) << 16) + ((_4) << 24))

// Character denoting the start of a new field
#define START_CHARACTER '\n'
// Character denoting the end of a field
#define END_CHARACTER '\r'
// Character denoting the split between a fields label and data
#define SPLIT_CHARACTER '\t'
// Character denoting an Async message
#define ASYNC_CHARACTER ':'
// Maximum number of fields that in a single block with a checksum
#define MAX_FIELDS_COUNT 22

// State of the Victron serial
enum {
    IDLE, LABEL, FIELD, ASYNC, CHECKSUM
};

// Field Label Identifiers. We convert 
enum {
    VOLTAGE = 'V',
    CURRENT = 'I',
    PANEL_VOLTAGE = BUILD_RAW_ID('V', 'P', 'V'),
    PANEL_POWER = BUILD_RAW_ID('P', 'P', 'V'),
    LOAD_CURRENT = BUILD_RAW_ID('I', 'L'),
    OPERATION_STATE = BUILD_RAW_ID('C', 'S'),
    ERROR_STATE = BUILD_RAW_ID('E', 'R', 'R'),
    LOAD = BUILD_RAW_ID('L', 'O', 'A', 'D'),
    YIELD_TOTAL = BUILD_RAW_ID('H', '1', '9'),
    YIELD_TODAY = BUILD_RAW_ID('H', '2', '0'),
    MAX_POWER_TODAY = BUILD_RAW_ID('H', '2', '1'),
    YIELD_YESTERDAY = BUILD_RAW_ID('H', '2', '2'),
    MAX_POWER_YESTERDAY = BUILD_RAW_ID('H', '2', '3'),
    PRODUCT_ID = BUILD_RAW_ID('P', 'I', 'D'),
    FIRMWARE = BUILD_RAW_ID('F', 'W'),
    SERIAL = BUILD_RAW_ID('S', 'E', 'R', '#'),
    TRACKER_OPERATION_MODE = BUILD_RAW_ID('M', 'P', 'P', 'T'),
    OFF_REASON = BUILD_RAW_ID('O', 'R'),
    DAY_SEQUENCE = BUILD_RAW_ID('H', 'S', 'D', 'S'),
    CHEC = BUILD_RAW_ID('C', 'H', 'E', 'C'),
    KSUM = BUILD_RAW_ID('K', 'S', 'U', 'M')
};

VictronSerial::VictronSerial() : initialized(false), serialPort(-1) { }

VictronSerial::~VictronSerial()
{
    dumpFields();
    closePort();
}

void VictronSerial::dumpFields()
{
    // Clear Queue
    while (!fields.empty()) 
    {
        delete fields.front();
        fields.pop();
    }
}

int VictronSerial::closePort()
{
    if (serialPort >= 0)
    {
        if (close(serialPort) < 0)
        {
            std::cout << "Error while trying to close the previous port. Error: " << std::strerror(errno) << "\n";
            return -1;
        }
    }
    return 0;
}

int VictronSerial::initialize(const char *port, int baud, char parity, int data_bit, int stop_bits)
{
    closePort();

    initialized = false;
    state = IDLE;
    checksum = 0;

    struct termios tty;
    serialPort = open(port, O_RDWR);

    // Check for errors
    if (serialPort < 0)
    {
        std::cout << "Error while trying to open port: " << port << ". Error: " << std::strerror(errno) << "\n";
        closePort();
        return -1;
    }

    if (tcgetattr(serialPort, &tty) != 0)
    {
        std::cout << "Error while trying to get parameters from the port: " << port << ". Error: " << std::strerror(errno) << "\n";
        closePort();
        return -1;
    }

    switch (parity)
    {
    case 'N':
        tty.c_cflag &= ~PARENB; // Clear parity
        break;
    case 'E':
        tty.c_cflag = (PARENB | tty.c_cflag) & ~PARODD; // Set parity and clear odd
        break;
    case 'O':
        tty.c_cflag |= (PARENB | PARODD);
        break;
    default:
        std::cout << "Error: " << parity << " parity is not supported. Supported parity: N, E, O\n"; // The one
        closePort();
        return -1;
    }

    switch (stop_bits)
    {
    case 1:
        tty.c_cflag &= ~CSTOPB; // Clear Stop bit. Aka 1 stop bit
        break;
    case 2:
        tty.c_cflag |= CSTOPB; // Set Stop bit. Aka 2 stop bits
        break;
    default:
        std::cout << "Error: " << stop_bits << " stop bits are not supported. Supported stop bits: 1, 2\n";
        closePort();
        return -1;
    }

    tty.c_cflag &= ~CSIZE; // Clear all the size bits
    
    switch (data_bit)
    {
    case 8:
        tty.c_cflag |= CS8; // 8 bits per byte
        break;
    case 7:
        tty.c_cflag |= CS7; // 7 bits per byte
        break;
    case 6:
        tty.c_cflag |= CS6; // 6 bits per byte
        break;
    case 5:
        tty.c_cflag |= CS5; // 5 bits per byte
        break;
    default:
        std::cout << "Error: " << data_bit << " data bits are not supported. Supported data bits: 5, 6, 7, 8\n";
        closePort();
        return -1;
    }

    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON; // Disabling Canoical mode

    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

     // No wait
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    switch (baud)
    {
    case B9600:
    case B19200:
    case B38400:
        cfsetspeed(&tty, baud);
        break;
    default:
        std::cout << "Error: Baud " << baud << " not supported. Supported Bauds: B9600, B19200, B38400\n";
        closePort();
        return -1;
    }

    // Save tty settings, also checking for error
    if (tcsetattr(serialPort, TCSANOW, &tty) != 0)
    {
        std::cout << "Error while trying to set parameters to the port: " << port << ". Error: " << std::strerror(errno) << "\n";
        closePort();
        return -1;
    }

    initialized = true;

    return 0;
}

void VictronSerial::processBuffer(char *buffer, int size)
{
    int index = 0;
    do
    {
        char input = buffer[index];
        switch (state)
        {
            case LABEL:
                if (input == SPLIT_CHARACTER)
                {
                    state = FIELD;
                    fieldIndex = 0;
                    memset(fieldData, 0, MAX_FIELD_LENGTH);
                }
                else if (input == ASYNC_CHARACTER)
                {
                    // Data we don't care about, ignore it
                    state = ASYNC;
                }
                else if (labelIndex >= MAX_LABEL_LENGTH)
                {
                    // Something went wrong and the label length is too long
                    // Return to idle
                    state = IDLE;
                }
                else
                {
                    labelData.buffer[labelIndex++] = input;
                }
                break;
            case FIELD:
                if (input == END_CHARACTER)
                {
                    state = IDLE;
                    // Process everything
                    processEntry();
                }
                else if (fieldIndex >= MAX_FIELD_LENGTH)
                {
                    // Something went wrong and the label length is too long
                    // Return to idle
                    state = IDLE;
                }
                else
                {
                    fieldData[fieldIndex++] = input;
                }
                break;
            case IDLE:
            case ASYNC:
                if (input == START_CHARACTER)
                {
                    state = LABEL;
                    memset(labelData.buffer, 0, MAX_LABEL_LENGTH);
                    labelIndex = MAX_LABEL_LENGTH;
                }
                break;
            default:
                break;
        }
        if (state != ASYNC)
        {
            checksum += input;
        }
    }
    while(index < size);
}

void VictronSerial::processEntry()
{
    // Victron specifies a max number of fields.
    // If we go over this count we might be overloading a queue.
    // We should dump our queue as we've got ourselves into a bad state.
    if (fields.size() >= MAX_FIELDS_COUNT)
    {
        dumpFields();
    }
    switch (labelData.lower)
    {
        case VOLTAGE:
        case PANEL_VOLTAGE:
            fields.push((Field*)(new Int32Field(labelData.lower, std::atol(fieldData))));
            break;
        case CURRENT:
        case PANEL_POWER:
        case LOAD_CURRENT:
        case YIELD_TOTAL:
        case YIELD_TODAY:
        case MAX_POWER_TODAY:
        case YIELD_YESTERDAY:
        case MAX_POWER_YESTERDAY:
        case DAY_SEQUENCE:
            fields.push((Field*)(new Int16Field(labelData.lower, std::atoi(fieldData))));
            break;
        case OPERATION_STATE:
        case ERROR_STATE:
        case TRACKER_OPERATION_MODE:
            fields.push((Field*)(new Int8Field(labelData.lower, std::atoi(fieldData))));
            break;
        case PRODUCT_ID:
        case FIRMWARE:
        case SERIAL:
            fields.push((Field*)(new StringField(labelData.lower, std::string(fieldData))));
            break;
        case LOAD: // ON/OFF value
            fields.push((Field*)(new Int8Field(labelData.lower, (strcmp(fieldData, "ON") != 0))));
            break;
        case CHEC:
            if (labelData.upper == KSUM)
            {
                if (checksum == fieldData[0])
                {
                    // Valid checksum
                }
                dumpFields();
                checksum = 0;
            }
        break; 
        case OFF_REASON:
        default:
            break;
    }
}

clock_t VictronSerial::doExecute()
{
    if (!initialized)
    {
        return IDLE_TIME;
    }

    char buffer[256];
    int bytes_read;

    bytes_read = read(serialPort, &buffer, sizeof(buffer));

    if (bytes_read > 0)
    {
        processBuffer(buffer, bytes_read);
    }
    
    return IDLE_TIME;
};