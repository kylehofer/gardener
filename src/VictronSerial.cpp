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
#include "utils.h"

#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <stdint.h>

#define IDLE_TIME (CLOCKS_PER_SEC << 2)

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

VictronSerial::VictronSerial() : Executor(), initialized(false), serialPort(-1), dataHandler(NULL) { }
VictronSerial::VictronSerial(VictronDataHandler* dataHandler) : Executor(), initialized(false), serialPort(-1), dataHandler(dataHandler) { }

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
    if (close_port(serialPort) < 0)
    {
        std::cout << "Error while trying to close the previous port. Error: " << std::strerror(errno) << "\n";
        return -1;
    }
    return 0;
}

int VictronSerial::initialize(const char *port, int baud, char parity, int data_bit, int stop_bits)
{
    closePort();

    initialized = false;
    state = IDLE;
    checksum = 0;

    serialPort = open_port(port, baud, parity, data_bit, stop_bits);

    if (serialPort < 0)
    {
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
        char input = buffer[index++];
        switch (state)
        {
            case LABEL:
                // std::cout << "LABEL\n";
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
                    labelIndex = 0;
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
            fields.push((Field*)(new Int8Field(labelData.lower, (strcmp(fieldData, "ON") == 0))));
            break;
        case CHEC:
            if (labelData.upper == KSUM)
            {
                if (checksum == 0 && dataHandler != NULL)
                {
                    // Valid checksum
                    dataHandler->fieldsUpdate(&fields);
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

    char buffer[1024];
    int bytes_read;
    bytes_read = read(serialPort, &buffer, sizeof(buffer));

    if (bytes_read > 0)
    {
        processBuffer(buffer, bytes_read);
    }
    
    return IDLE_TIME;
};