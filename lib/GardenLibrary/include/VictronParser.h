/*
 * File: VictronParser.h
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

#ifndef VICTRONPARSER
#define VICTRONPARSER

#include "Fields.h"
#ifndef __AVR__
#include <cstdint>
#else
#include <Arduino.h>
#endif // __AVR__

// Max length of a Victron Field Label
#define MAX_LABEL_LENGTH 8
// Max length of a Victron Field Value
#define MAX_FIELD_LENGTH 32
// Maximum number of fields that in a single block with a checksum
#define MAX_FIELDS_COUNT 22

// Convert a set of characters into a unique binary ID
#define BUILD_RAW_ID(...) BUILD_RAW_ID_ARG(__VA_ARGS__, 0, 0, 0, 0)
#define BUILD_RAW_ID_ARG(_1, _2, _3, _4, ...) ((_1) + ((_2) << 8) + ((unsigned long)(_3) << 16) + ((unsigned long)(_4) << 24))

// State of the Victron serial
enum {
    IDLE, LABEL, FIELD, ASYNC, CHECKSUM
};

// Field Label Identifiers. We convert 
const uint32_t VOLTAGE = 'V';
const uint32_t CURRENT = 'I';
const uint32_t PANEL_VOLTAGE = BUILD_RAW_ID('V', 'P', 'V');
const uint32_t PANEL_POWER = BUILD_RAW_ID('P', 'P', 'V');
const uint32_t LOAD_CURRENT = BUILD_RAW_ID('I', 'L');
const uint32_t OPERATION_STATE = BUILD_RAW_ID('C', 'S');
const uint32_t ERROR_STATE = BUILD_RAW_ID('E', 'R', 'R');
const uint32_t LOAD = BUILD_RAW_ID('L', 'O', 'A', 'D');
const uint32_t YIELD_TOTAL = BUILD_RAW_ID('H', '1', '9');
const uint32_t YIELD_TODAY = BUILD_RAW_ID('H', '2', '0');
const uint32_t MAX_POWER_TODAY = BUILD_RAW_ID('H', '2', '1');
const uint32_t YIELD_YESTERDAY = BUILD_RAW_ID('H', '2', '2');
const uint32_t MAX_POWER_YESTERDAY = BUILD_RAW_ID('H', '2', '3');
const uint32_t PRODUCT_ID = BUILD_RAW_ID('P', 'I', 'D');
const uint32_t FIRMWARE = BUILD_RAW_ID('F', 'W');
const uint32_t SERIAL_NUMBER = BUILD_RAW_ID('S', 'E', 'R', '#');
const uint32_t TRACKER_OPERATION_MODE = BUILD_RAW_ID('M', 'P', 'P', 'T');
const uint32_t OFF_REASON = BUILD_RAW_ID('O', 'R');
const uint32_t DAY_SEQUENCE = BUILD_RAW_ID('H', 'S', 'D', 'S');
const uint32_t CHEC = BUILD_RAW_ID('C', 'h', 'e', 'c');
const uint32_t KSUM = BUILD_RAW_ID('k', 's', 'u', 'm');


// Used to convert Field labels to binary values for fast comparisons
typedef union {
    unsigned char buffer[MAX_LABEL_LENGTH];
    struct {
        uint32_t lower;
        uint32_t upper;
    }; 
} LabelToBuffer_u;

class VictronFieldHandler
{
private:
protected:
public:
    // VictronFieldHandler() {};
    // virtual ~VictronFieldHandler() {};
    virtual void fieldUpdate(uint32_t id, void* data, size_t size) = 0;
};

class VictronParser
{
private:
    int state;
    bool initialized;
    int8_t checksum;
    LabelToBuffer_u labelData;
    int labelIndex;
    char fieldData[MAX_FIELD_LENGTH];
    int fieldIndex;
    Field* fields[MAX_FIELDS_COUNT];
    void (*fieldHandlerFunc)(uint32_t, void*, size_t);
    VictronFieldHandler* fieldHandlerClass;
    int fieldCount;

    /**
     * @brief Process an single field line containing a Label/Data combo
     * 
     */
    void processEntry();

    /**
     * @brief Processes all fields in the queue
     * 
     */
    void processFields();

    /**
     * @brief Frees all memory held by the fields queue
     * 
     */
    void dumpFields();

protected:
public:
    VictronParser();
    VictronParser(VictronFieldHandler* dataHandler);
    VictronParser(void (*function)(uint32_t, void*, size_t));
    // void victronDataHandler(uint32_t id, void *data, size_t size)
    ~VictronParser();

    /**
     * @brief Process a buffer read from the victron serial
     * 
     * @param buffer buffer read from the serial
     * @param size The number of bytes read
     */
    void parse(const char *buffer, int size);
};

#endif /* VICTRONPARSER */
