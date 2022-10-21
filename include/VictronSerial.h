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

// Used to convert Field labels to binary values for fast comparisons
typedef union {
    char buffer[MAX_LABEL_LENGTH];
    struct {
        uint32_t upper;
        uint32_t lower;
    }; 
} LabelToBuffer_u;

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
    uint8_t checksum;
    std::queue<Field*> fields;

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
