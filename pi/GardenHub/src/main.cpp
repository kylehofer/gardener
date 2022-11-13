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

#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <csignal>
#include <atomic>

#include "GardenBedClient.h"
#include "GardenShedClient.h"
#include "ModbusConnection.h"

#define MODBUS_PORT "/dev/ttySC0"
#define MODBUS_BAUD 38400
#define MODBUS_DATA_BITS 8
#define MODBUS_STOP_BITS 2
#define MODBUS_PARITY 'N'

#define MODBUS_ENABLED

using namespace std;

vector<thread> threads;

void gardenBedRunner(ModbusConnection* modbusConnection)
{
    GardenBedClient gardenBed = GardenBedClient(modbusConnection);

    for(;;) { gardenBed.executeSync(); }
}

void gardenShedRunner(ModbusConnection* modbusConnection)
{
    GardenShedClient gardenShed = GardenShedClient(modbusConnection);

    for(;;) { gardenShed.executeSync(); }
}

int main(int argc, char *argv[])
{
    ModbusConnection modbusConnection;

    if (modbusConnection.configure(MODBUS_PORT, MODBUS_BAUD, MODBUS_PARITY, MODBUS_DATA_BITS, MODBUS_STOP_BITS) != 0)
    {
        exit(EXIT_FAILURE);
    }

    if (modbusConnection.connect() != 0)
    {
        exit(EXIT_FAILURE);
    }

    #ifdef MODBUS_ENABLED
    threads.push_back(thread(gardenBedRunner, &modbusConnection));
    threads.push_back(thread(gardenShedRunner, &modbusConnection));
    #endif

    for(;;) { this_thread::sleep_for(std::chrono::seconds(1)); }

    return 0;
}
