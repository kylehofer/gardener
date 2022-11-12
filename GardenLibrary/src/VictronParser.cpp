/*
 * File: VictronParser.cpp
 * Project: gardener
 * Created Date: Monday November 7th 2022
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

#include "VictronParser.h"

#include "Fields.h"
#ifndef __AVR__
#include <cstdlib>
using namespace std;
#else
#include <Arduino.h>
#endif // __AVR__

// Character denoting the start of a new field
#define START_CHARACTER '\n'
// Character denoting the end of a field
#define END_CHARACTER '\r'
// Character denoting the split between a fields label and data
#define SPLIT_CHARACTER '\t'
// Character denoting an Async message
#define ASYNC_CHARACTER ':'


VictronParser::VictronParser() : fieldHandlerFunc(NULL), fieldHandlerClass(NULL), fieldCount(0) { }
VictronParser::VictronParser(VictronFieldHandler* fieldHandlerClass) : fieldHandlerFunc(NULL), fieldHandlerClass(fieldHandlerClass), fieldCount(0) { };
VictronParser::VictronParser(void (*fieldHandlerFunc)(uint32_t, void*, size_t)) : fieldHandlerFunc(fieldHandlerFunc), fieldHandlerClass(NULL), fieldCount(0) { };

VictronParser::~VictronParser()
{
    dumpFields();
}

void VictronParser::dumpFields()
{
    for (int i = (fieldCount - 1); i >= 0; i--)
    {
        delete fields[i];
    }
    
    fieldCount = 0;
}

void VictronParser::processFields()
{
    if (!fieldHandlerFunc && !fieldHandlerClass)
    {
        dumpFields();
        return;
    }
    
    Field* field;

    if (fieldHandlerClass)
    {
        for (int i = (fieldCount - 1); i >= 0; i--)
        {
            field = fields[i];
            fieldHandlerClass->fieldUpdate(field->getId(), field->getData(), field->getSize());
            delete field;
        }
    }
    else
    {
        for (int i = (fieldCount - 1); i >= 0; i--)
        {
            field = fields[i];
            fieldHandlerFunc(field->getId(), field->getData(), field->getSize());
            delete field;
        }
    }

    fieldCount = 0;
}

void VictronParser::parse(const char *buffer, int size)
{
    int index = 0;
    do
    {
        char input = buffer[index++];
        switch (state)
        {
            case LABEL:
                // cout << "LABEL\n";
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

void VictronParser::processEntry()
{
    // Victron specifies a max number of fields.
    // If we go over this count we might be overloading a queue.
    // We should dump our queue as we've got ourselves into a bad state.
    if (fieldCount >= MAX_FIELDS_COUNT)
    {
        dumpFields();
    }
    
    switch (labelData.lower)
    {
        case VOLTAGE:
        case PANEL_VOLTAGE:
            fields[fieldCount] = new Field(labelData.lower, atol(fieldData));
            fieldCount++;
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
            fields[fieldCount] = new Field(labelData.lower, atoi(fieldData));
            fieldCount++;
            break;
        case OPERATION_STATE:
        case ERROR_STATE:
        case TRACKER_OPERATION_MODE:
            fields[fieldCount] = new Field(labelData.lower, (int8_t) atoi(fieldData));
            fieldCount++;
            break;
        case PRODUCT_ID:
        case FIRMWARE:
        case SERIAL_NUMBER:
            fields[fieldCount] = new Field(labelData.lower, fieldData, fieldIndex + 1);
            fieldCount++;
            break;
        case LOAD: // ON/OFF value
            fields[fieldCount] = new Field(labelData.lower, (bool) (strcmp(fieldData, "ON") == 0));
            fieldCount++;
            break;
        case CHEC:
            if (labelData.upper == KSUM)
            {
                if (checksum == 0)
                {
                    processFields();
                }
                else
                {
                    dumpFields();
                }
                checksum = 0;
            }
        break; 
        case OFF_REASON:
        default:
            break;
    }
}