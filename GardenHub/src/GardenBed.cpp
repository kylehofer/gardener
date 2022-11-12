/*
 * File: GardenBed.cpp
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

#include "GardenBed.h"
#include "GardenBedCommon.h"
#include <iostream>

#define LIGHT_HIGH 45
#define LIGHT_LOW 0

// This is a delay to prevent polling/writing at too high of a frequency
// This is chosen purely from testing and hasn't yet been chosen by calculations
#define EXECUTE_TIME 200
#define GARDEN_BED "Garden Bed: " <<

GardenBed::GardenBed() : ModbusDevice() {};
GardenBed::GardenBed(ModbusConnection* connection) : ModbusDevice(connection, MODBUS_ID, modbus_mapping_new(0, 0, 0, TOTAL_HOLDING_REGISTERS)) {};

int32_t GardenBed::doExecute()
{
    int request_result;

    request_result = request();

    if (request_result < 0)
    {
        // TODO: Error
    }

    if (request_result > 0)
    {
        time_t current_time = time(NULL);
        struct tm local_time = *localtime(&current_time);
        uint16_t* holdingRegisters = getHoldingRegisters();

        if ( // Make Time Range Configurable
            (local_time.tm_hour > 20 || (local_time.tm_hour == 20 && local_time.tm_min >= 00)) && 
            (local_time.tm_hour < 22 || (local_time.tm_hour == 22 && local_time.tm_min < 00))
        )
        {
            if (holdingRegisters[GARDEN_LIGHT_COMMAND] != LIGHT_HIGH)
            {
                holdingRegisters[GARDEN_LIGHT_COMMAND] = LIGHT_HIGH;
                std::cout << GARDEN_BED "sunset activated, setting expected intensity to " << LIGHT_HIGH << "\%\n";
            }
        }
        else if (holdingRegisters[GARDEN_LIGHT_COMMAND] != LIGHT_LOW)
        {
            holdingRegisters[GARDEN_LIGHT_COMMAND] = LIGHT_LOW;
            std::cout << GARDEN_BED "Saving power, setting expected intensity to " << LIGHT_LOW << "\%\n";
        }

        reply();
    }

    return EXECUTE_TIME;
}