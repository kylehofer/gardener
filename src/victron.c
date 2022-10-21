/*
 * File: victron.c
 * Project: gardener
 * Created Date: Monday October 17th 2022
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

#include "victron.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

//serial_client = serial.Serial('/dev/ttyACM0', 115200, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE, timeout=3, write_timeout=3)

#define BAUD B19200
#define START_CHARACTER '\n'
#define END_CHARACTER '\r'
#define SPLIT_CHARACTER '\t'
#define ASYNC_CHARACTER ':'
#define MAX_LABEL_LENGTH 8

enum {
    IDLE, LABEL, FIELD//, CHEC, KSUM
};

// #define SUB_ID_TO_RAW(ID, i) (sizeof(ID) > (i + 1)) * (ID[i] << (i * 8))
// #define ID_TO_RAW(ID) (SUB_ID_TO_RAW(ID, 0)) + (SUB_ID_TO_RAW(ID, 1)) + (SUB_ID_TO_RAW(ID, 2)) + (SUB_ID_TO_RAW(ID, 3))
// #define ID_TO_RAW2(ID) (ID[0]) + (ID[1] << 8) + (ID[2] << 16) + (ID[3] << 24)

// #define TO_BINARY(C, I) (C << (I * 8))
// #define TO_BINARY(C, I) (C << (I * 8))

#define BUILD_RAW_ID(...) BUILD_RAW_ID_ARG(__VA_ARGS__, 0, 0, 0, 0)
#define BUILD_RAW_ID_ARG(_1, _2, _3, _4, ...) ((_1) + ((_2) << 8) + ((_3) << 16) + ((_4) << 24))


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

// Union used for converting door data to a buffer for transmissions
typedef union {
    char buffer[MAX_LABEL_LENGTH];
    struct {
        uint32_t lower;
        uint32_t upper;
    }; 
} LabelToBuffer_u;

int state;
int serial_port;

uint8_t checksum_calculator;
LabelToBuffer_u label;
int label_index;
char field_buffer[24];
int field_index;


int victron_init()
{
    state = IDLE;

    struct termios tty;

    serial_port = open("/dev/ttyO5", O_RDWR);

    // Check for errors
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return -1;
    }

    if(tcgetattr(serial_port, &tty) != 0) {
        victron_destroy();
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    // Grabbed from https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)

    tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
    tty.c_cflag |= CS8; // 8 bits per byte (most common)

    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON; // Disabling Canoical mode

    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 0; // No wait
    tty.c_cc[VMIN] = 0;

    cfsetspeed(&tty, BAUD);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        victron_destroy();
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}

void process_buffer(char *buffer, int size)
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
                    field_index = 0;
                }
                else if (input == ASYNC_CHARACTER)
                {
                    // Data we don't care about, ignore it
                    state = IDLE;
                }
                else if (--label_index < 0)
                {
                    // Something went wrong and the field length is too long
                    // Return to idle
                    state = IDLE;
                }
                else
                {
                    label.buffer[label_index] = input;
                }
                break;
            case FIELD:
                if (input == END_CHARACTER)
                {
                    state = LABEL;
                    memset(label.buffer, 0, MAX_LABEL_LENGTH);
                    label_index = MAX_LABEL_LENGTH;
                }
                else
                {
                    field_buffer[field_index++] = input;
                }
                break;
            case IDLE:
                if (input == START_CHARACTER)
                {
                    state = LABEL;
                    memset(label.buffer, 0, MAX_LABEL_LENGTH);
                    label_index = MAX_LABEL_LENGTH;
                }
                break;
            default:
                break;
        }
        if (state != IDLE)
        {
            checksum_calculator += input;
        }
    }
    while(index < size);
}


int victron_process()
{
    char buffer[256];
    int bytes_read;
    

    while ((bytes_read = read(serial_port, &buffer, sizeof(buffer))) > 0)
    {
        process_buffer(buffer, bytes_read);
    }
    return 0;
}

void victron_destroy()
{
    close(serial_port);
}

