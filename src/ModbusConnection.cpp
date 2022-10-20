/*
 * File: ModbusConnection.cpp
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

#include "ModbusConnection.h"
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <iostream>

ModbusConnection::ModbusConnection() : connected(false), modbusContext(NULL) { }

ModbusConnection::~ModbusConnection() 
{
    if (this->modbusContext == NULL)
    {
        return;
    }
    
    if (this->connected)
    {
        this->disconnect();
    }

    modbus_free(this->modbusContext);
}

int ModbusConnection::configure(const char *port, int baud, char parity, int data_bit, int stop_bit)
{
    modbusContext = modbus_new_rtu(port, baud, parity, data_bit, stop_bit);
    if (modbusContext == NULL)
    {
        std::cout << "Unable to create the modbus context for the port: " << port << ". Error: " << std::strerror(errno) << "\n";
        return -1;
    }

    this->port = port;

    #if (LIBMODBUS_VERSION_MINOR > 0)
         modbus_set_response_timeout(modbusContext, 0, 500000);
    #else
        struct timeval response_timeout;
        response_timeout.tv_sec = 0;
        response_timeout.tv_usec = 500000;
        modbus_set_response_timeout(modbusContext, &response_timeout);
    #endif
    return 0;
}

int ModbusConnection::connect()
{
    if (modbusContext == NULL)
    {
        std::cout << "ModbusConnection is not configured.\n";
        return -1;
    }

    int result = modbus_connect(modbusContext);
    if(result < 0)
    {
        std::cout << "Error while trying to connect to port: " << port << ". Error: " << std::strerror(errno) << "\n";
        return result;
    }
    connected = true;
    return 0;
}

void ModbusConnection::disconnect()
{
    modbus_close(modbusContext);
    connected = true;
}

int ModbusConnection::setSlaveId(int slaveId)
{
    int result;

    if (slaveId != this->slaveId)
    {
        result = modbus_set_slave(modbusContext, slaveId);
        if(result < 0)
        {
            std::cout << "Error while trying to set slave id to: " << slaveId << ". Error: " << std::strerror(errno) << "\n";
            return result;
        }
        this->slaveId = slaveId;
    }
    return 0;
}

int ModbusConnection::read(int slaveId, int address, int size, uint16_t* value)
{
    return this->setSlaveId(slaveId) || modbus_read_registers(modbusContext, 0, size, value);
}

int ModbusConnection::write(int slaveId, int address, uint16_t value)
{
    return this->setSlaveId(slaveId) || modbus_write_register(modbusContext, address, value);
}

int ModbusConnection::write(int slaveId, int address, int size, uint16_t* data)
{
    return this->setSlaveId(slaveId) || modbus_write_registers(modbusContext, address, size, data);
}
