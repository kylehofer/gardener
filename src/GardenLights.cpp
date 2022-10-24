/*
 * File: GardenLights.cpp
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

#include "GardenLights.h"
#include <time.h>

#define LIGHT_HIGH 85
#define SLAVE_ID 2

// This is a delay to prevent polling/writing at too high of a frequency
// This is chosen purely from testing and hasn't yet been chosen by calculations
#define EXECUTE_TIME (CLOCKS_PER_SEC << 2)
#define IDLE_TIME (CLOCKS_PER_SEC << 8)

enum {
    LIGHT_COMMAND_ADDRESS,
    TOTAL_ERRORS,
    TOTAL_REGS_SIZE
};

GardenLights::GardenLights() : ModbusDevice(), expectedLightCommand(0), readLightCommand(0) {};
GardenLights::GardenLights(ModbusConnection* connection) : ModbusDevice(connection, SLAVE_ID), expectedLightCommand(0), readLightCommand(0) {};

clock_t GardenLights::doExecute()
{
    time_t current_time = time(NULL);
    struct tm local_time = *localtime(&current_time);

    if ( // Make Time Range Configurable
            (local_time.tm_hour > 18 || (local_time.tm_hour == 18 && local_time.tm_min >= 30)) && 
            (local_time.tm_hour < 22 || (local_time.tm_hour == 22 && local_time.tm_min < 30))
    )
    {
        this->expectedLightCommand = LIGHT_HIGH;
    }
    else
    {
        this->expectedLightCommand = 0;
    }

    if (this->expectedLightCommand != this->readLightCommand)
    {
        if (this->write(LIGHT_COMMAND_ADDRESS, this->expectedLightCommand))
        {
            // TODO: Failed to write
        }
        return EXECUTE_TIME;
    }
    uint16_t data[TOTAL_REGS_SIZE];

    if (!this->read(0, TOTAL_REGS_SIZE, data))
    {
        this->readLightCommand = data[LIGHT_COMMAND_ADDRESS];
    }
    else
    {
        // TODO: Failed to read
    }
    return EXECUTE_TIME;
}