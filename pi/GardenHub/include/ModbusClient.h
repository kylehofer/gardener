/*
 * File: ModbusClient.h
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

#ifndef MODBUSCLIENT
#define MODBUSCLIENT

#include <modbus.h>
#include "ModbusClient.h"
#include "ModbusDevice.h"
#include "ModbusConnection.h"

enum ModbusClientState
{
    POLLED, WRITTEN, IDLE
};

class ModbusClient : ModbusDevice
{
private:
    ModbusClientState state;
protected:
    int readBits(int address, int size, uint8_t* data);
    int readInputBits(int address, int size, uint8_t* data);
    int readRegisters(int address, int size, uint16_t* data);
    int readInputRegisters(int address, int size, uint16_t* data);
    int writeBit(int address, int value);
    int writeBits(int address, int size, uint8_t* values);
    int writeRegister(int address, uint16_t value);
    int writeRegisters(int address, int size, uint16_t* values);
public:
    ModbusClient();
    ModbusClient(ModbusConnection* connection, int slaveId);
};

#endif /* MODBUSDEVICE */
