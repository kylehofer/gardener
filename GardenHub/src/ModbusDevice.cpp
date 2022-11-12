/*
 * File: ModbusDevice.cpp
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

#include "ModbusDevice.h"
#include <cstddef>

ModbusDevice::ModbusDevice() : connection(NULL), slaveId(-1), modbusMapping(NULL) { }

ModbusDevice::ModbusDevice(ModbusConnection* connection, int slaveId, modbus_mapping_t* modbusMapping) : connection(connection), slaveId(slaveId), modbusMapping(modbusMapping) { }

void ModbusDevice::setConnection(ModbusConnection* connection)
{
    this->connection = connection;
}

void ModbusDevice::setSlaveId(int slaveId)
{
    this->slaveId = slaveId;
}

int ModbusDevice::request()
{
    return (modbusRequestResult = connection->request(slaveId, modbusRequest));
}

int ModbusDevice::reply()
{
    return connection->reply(slaveId, modbusRequest, modbusRequestResult, modbusMapping);
}

uint8_t* ModbusDevice::getCoils()
{
    return modbusMapping->tab_bits;
}

uint8_t* ModbusDevice::getDiscreteInputs()
{
    return modbusMapping->tab_input_bits;
}
uint16_t* ModbusDevice::getInputRegisters()
{
    return modbusMapping->tab_registers;
}
uint16_t* ModbusDevice::getHoldingRegisters()
{
    return modbusMapping->tab_input_registers;
}

// int ModbusDevice::read(int address, int size, uint16_t* value)
// {
//     return connection->read(this->slaveId, address, size, value);
// }

// int ModbusDevice::write(int address, uint16_t value)
// {
//     return connection->write(this->slaveId, address, value);
// }

// int ModbusDevice::write(int address, int size, uint16_t* value)
// {
//     return connection->write(this->slaveId, address, size, value);
// }