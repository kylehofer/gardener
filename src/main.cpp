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

// #include <cstdlib>
#include <unistd.h>


#include "GardenLights.h"
#include "ModbusConnection.h"
// #include "victron.h"

#define PORT "/dev/ttyO4"
#define BAUD 38400
#define DATA_BITS 8
#define STOP_BITS 2
#define PARITY 'N'


int main(int argc, char *argv[])
{
    ModbusConnection connection;
    connection = ModbusConnection();

    if (connection.configure(PORT, BAUD, PARITY, DATA_BITS, STOP_BITS) != 0)
    {
        exit(EXIT_FAILURE);
    }

    if (connection.connect() != 0)
    {
        exit(EXIT_FAILURE);
    }

    GardenLights gardenLights = GardenLights(&connection);
    
    for(;;)
    {
        gardenLights.execute();
        usleep(50);
    }
    return 0;
}

// void exit(int status)
// {
//     // garden_lights_destroy();
// }