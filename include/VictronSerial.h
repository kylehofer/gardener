/*
 * File: VictronSerial.h
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

#ifndef VICTRONSERIAL
#define VICTRONSERIAL

#include "Executor.h"
#include "Fields.h"
#include <cstdint>
#include <queue>

// Max length of a Victron Field Label
#define MAX_LABEL_LENGTH 8
// Max length of a Victron Field Value
#define MAX_FIELD_LENGTH 32

// Convert a set of characters into a unique binary ID
#define BUILD_RAW_ID(...) BUILD_RAW_ID_ARG(__VA_ARGS__, 0, 0, 0, 0)
#define BUILD_RAW_ID_ARG(_1, _2, _3, _4, ...) ((_1) + ((_2) << 8) + ((_3) << 16) + ((_4) << 24))

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
    CHEC = BUILD_RAW_ID('C', 'h', 'e', 'c'),
    KSUM = BUILD_RAW_ID('k', 's', 'u', 'm')
};

// Used to convert Field labels to binary values for fast comparisons
typedef union {
    unsigned char buffer[MAX_LABEL_LENGTH];
    struct {
        uint32_t lower;
        uint32_t upper;        
    }; 
} LabelToBuffer_u;

class VictronDataHandler
{
private:
protected:
public:
    VictronDataHandler() {};
    virtual ~VictronDataHandler() {};
    virtual void fieldsUpdate(std::queue<Field*>* fields) = 0;
};

class VictronSerial : Executor
{
private:
    int state;
    bool initialized;
    int serialPort;
    LabelToBuffer_u labelData;
    int labelIndex;
    char fieldData[MAX_FIELD_LENGTH];
    int fieldIndex;
    int8_t checksum;
    std::queue<Field*> fields;
    VictronDataHandler* dataHandler;

    /**
     * @brief Process a buffer read from the victron serial
     * 
     * @param buffer buffer read from the serial
     * @param size The number of bytes read
     */
    void processBuffer(char *buffer, int size);

    /**
     * @brief Process an single field line containing a Label/Data combo
     * 
     */
    void processEntry();

    /**
     * @brief Closes the serial port
     * 
     * @return int result from closing the port
     */
    int closePort();

    /**
     * @brief Frees all memory held by the fields queue
     * 
     */
    void dumpFields();

protected:
    /**
     * @brief Reads available data from the victron serial
     * 
     * @return clock_t 
     */
    clock_t doExecute();

public:
    VictronSerial();
    VictronSerial(VictronDataHandler* dataHandler);
    ~VictronSerial();
    using Executor::execute;

    /**
     * @brief Configures and opens a serial port
     * 
     * @param device 
     * @param baud 
     * @param parity 
     * @param data_bit 
     * @param stop_bit 
     * @return int non-zero return if the serial port failed to open or configure
     */
    int initialize(const char *device, int baud, char parity, int data_bit, int stop_bit);


};

#endif /* VICTRONSERIAL */
