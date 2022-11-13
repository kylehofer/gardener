/*
 * File: ModbusClient.cpp
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

#include "ModbusClient.h"
#include <cstddef>

ModbusClient::ModbusClient() : ModbusDevice(), state(IDLE) { }

ModbusClient::ModbusClient(ModbusConnection* connection, int slaveId) : ModbusDevice(connection, slaveId), state(IDLE) { }

int ModbusClient::readBits(int address, int size, uint8_t* data)
{
    return connection->readBits(slaveId, address, size, data);
}

int ModbusClient::readInputBits(int address, int size, uint8_t* data)
{
    return connection->readInputBits(slaveId, address, size, data);
}

int ModbusClient::readRegisters(int address, int size, uint16_t* data)
{
    return connection->readRegisters(slaveId, address, size, data);
}

int ModbusClient::readInputRegisters(int address, int size, uint16_t* data)
{
    return connection->readInputRegisters(slaveId, address, size, data);
}

int ModbusClient::writeBit(int address, int value)
{
    return connection->writeBit(slaveId, address, value);
}

int ModbusClient::writeBits(int address, int size, uint8_t* values)
{
    return connection->writeBits(slaveId, address, size, values);
}

int ModbusClient::writeRegister(int address, uint16_t value)
{
    return connection->writeRegister(slaveId, address, value);
}

int ModbusClient::writeRegisters(int address, int size, uint16_t* values)
{
    return connection->writeRegisters(slaveId, address, size, values);
}