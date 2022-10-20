/*
 * File: ModbusDevice.h
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

#ifndef MODBUSDEVICE
#define MODBUSDEVICE

#include <modbus.h>
#include "ModbusConnection.h"

class ModbusDevice
{
private:
    ModbusConnection* connection;
    clock_t pollDelay;
    int slaveId;

protected:
    /**
     * @brief Attempt to read data for the device
     * 
     * @param address 
     * @param size 
     * @param value 
     * @return int 
     */
    int read(int address, int size, uint16_t* value);

    /**
     * @brief Attempt to write a byte data to the device
     * 
     * @param address 
     * @param value 
     * @return int 
     */
    int write(int address, uint16_t value);
    
    /**
     * @brief Attempt to write bytes of data to the device
     * 
     * @param address 
     * @param size 
     * @param value 
     * @return int 
     */
    int write(int address, int size, uint16_t* value);
public:
    ModbusDevice();
    ModbusDevice(ModbusConnection* connection, int slaveId);
    /**
     * @brief Set the Connection
     * 
     * @param connection 
     */
    void setConnection(ModbusConnection* connection);
    /**
     * @brief Set the Slave Id
     * 
     * @param slaveId 
     */
    void setSlaveId(int slaveId);
};

#endif /* MODBUSDEVICE */
