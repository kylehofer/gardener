/*
 * File: GardenBedClient.cpp
 * Project: gardener
 * Created Date: Thursday October 20th 2022
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

#include "GardenBedClient.h"
#include "GardenBedCommon.h"
#include <iostream>

using namespace GardenBed;

#define LIGHT_HIGH 45
#define LIGHT_LOW 0

// This is a delay to prevent polling/writing at too high of a frequency
// This is chosen purely from testing and hasn't yet been chosen by calculations
#define POLL_TIME 5
#define GARDEN_BED "Garden Bed: " <<

GardenBedClient::GardenBedClient() : ModbusClient() {};
GardenBedClient::GardenBedClient(ModbusConnection* connection) : ModbusClient(connection, MODBUS_ID) {};

int32_t GardenBedClient::doExecute()
{
    uint16_t holdingRegisters[TOTAL_HOLDING_REGISTERS];
    
    int result;

    result = readRegisters(MODBUS_START_REGISTER, TOTAL_HOLDING_REGISTERS, holdingRegisters);

    if (result < 0)
    {
        // return POLL_TIME;
    }

    time_t current_time = time(NULL);
    struct tm local_time = *localtime(&current_time);

    if ( // Make Time Range Configurable
        (local_time.tm_hour > 20 || (local_time.tm_hour == 20 && local_time.tm_min >= 00)) && 
        (local_time.tm_hour < 22 || (local_time.tm_hour == 22 && local_time.tm_min < 00))
    )
    {
        if (holdingRegisters[GARDEN_LIGHT_COMMAND] != LIGHT_HIGH)
        {
            writeRegister(GARDEN_LIGHT_COMMAND + MODBUS_START_REGISTER, LIGHT_HIGH);
            std::cout << GARDEN_BED "sunset activated, setting expected intensity to " << LIGHT_HIGH << "\%\n";
        }
    }
    else if (holdingRegisters[GARDEN_LIGHT_COMMAND] != LIGHT_LOW)
    {
        writeRegister(GARDEN_LIGHT_COMMAND + MODBUS_START_REGISTER, LIGHT_LOW);
        std::cout << GARDEN_BED "Saving power, setting expected intensity to " << LIGHT_LOW << "\%\n";
    }

    return POLL_TIME;
}