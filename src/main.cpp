/*
 * File: main.c
 * Project: gardener
 * Created Date: Sunday October 16th 2022
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

#include <ctime>
#include <cstdlib>

#include <termios.h>
#include <unistd.h>


#include "GardenLights.h"
#include "ModbusConnection.h"
#include "VictronSerial.h"
#include "CanNetwork.h"

#define MODBUS_PORT "/dev/ttyO4"
#define MODBUS_BAUD 38400
#define MODBUS_DATA_BITS 8
#define MODBUS_STOP_BITS 2
#define MODBUS_PARITY 'N'

#define VICTRON_PORT "/dev/ttyO5"
#define VICTRON_BAUD B19200
#define VICTRON_DATA_BITS 8
#define VICTRON_STOP_BITS 1
#define VICTRON_PARITY 'N'

// #define MODBUS_ENABLED
// #define VICTRON_ENABLED
#define CAN_ENABLED


int main(int argc, char *argv[])
{
    ModbusConnection modbusConnection = ModbusConnection();
    VictronSerial victronSerial = VictronSerial();
    CanNetwork canNetwork = CanNetwork();

    #ifdef MODBUS_ENABLED
    if (modbusConnection.configure(MODBUS_PORT, MODBUS_BAUD, MODBUS_PARITY, MODBUS_DATA_BITS, MODBUS_STOP_BITS) != 0)
    {
        exit(EXIT_FAILURE);
    }

    if (modbusConnection.connect() != 0)
    {
        exit(EXIT_FAILURE);
    }
    GardenLights gardenLights = GardenLights(&modbusConnection);
    #endif

    

    #ifdef VICTRON_ENABLED
    if (victronSerial.initialize(VICTRON_PORT, VICTRON_BAUD, VICTRON_PARITY, VICTRON_DATA_BITS, VICTRON_STOP_BITS))
    {
        exit(EXIT_FAILURE);
    }
    #endif

    #ifdef CAN_ENABLED
    if (canNetwork.initialize())
    {
        exit(EXIT_FAILURE);
    }
    #endif
    
    for(;;)
    {
        #ifdef MODBUS_ENABLED
            gardenLights.execute();
        #endif

        #ifdef VICTRON_ENABLED
            victronSerial.execute();
        #endif

        #ifdef CAN_ENABLED
            canNetwork.execute();
        #endif
        usleep(50);
    }
    return 0;
}

// void exit(int status)
// {
//     // garden_lights_destroy();
// }