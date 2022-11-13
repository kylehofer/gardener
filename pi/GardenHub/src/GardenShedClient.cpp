/*
 * File: GardenShedClient.cpp
 * Project: gardener
 * Created Date: Saturday November 12th 2022
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

#include "GardenShedClient.h"
#include "GardenShedCommon.h"
#include <iostream>

using namespace GardenShed;

#define LIGHT_HIGH 15
#define LIGHT_LOW 0

// This is a delay to prevent polling/writing at too high of a frequency
// This is chosen purely from testing and hasn't yet been chosen by calculations
#define POLL_TIME 500
#define GARDEN_SHED "Garden Shed: " <<

GardenShedClient::GardenShedClient() : ModbusClient() {};
GardenShedClient::GardenShedClient(ModbusConnection* connection) : ModbusClient(connection, MODBUS_ID) {};

int32_t GardenShedClient::doExecute()
{
    uint16_t registers[TOTAL_HOLDING_REGISTERS];
    uint16_t inputRegisters[TOTAL_INPUT_REGISTERS];
    
    int result;

    result = readInputRegisters(MODBUS_START_REGISTER, TOTAL_INPUT_REGISTERS, inputRegisters);

    if (result < 0)
    {
        // return POLL_TIME;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(POLL_TIME));


    result = readRegisters(MODBUS_START_REGISTER, TOTAL_HOLDING_REGISTERS, registers);

    if (result < 0)
    {
        // return POLL_TIME;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(POLL_TIME));

    if (registers[SHED_LIGHT_COMMAND] != LIGHT_HIGH)
    {
        writeRegister(SHED_LIGHT_COMMAND + MODBUS_START_REGISTER, LIGHT_HIGH);
    }

    return POLL_TIME;
}