/*
 * File: ModbusConnection.h
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

#ifndef MODBUSCONNECTION
#define MODBUSCONNECTION

#include <modbus.h>
#include <mutex>
#include <chrono>
#include <thread>
using namespace std;

/**
 * @brief 
 * 
 */
class ModbusConnection
{
private:
    int slaveId;
    bool connected;
    modbus_t *modbusContext;
    const char *port;
    mutex connectionLock;
    int setSlaveId(int slaveId);
    void lock();
    void unlock();
protected:

public:
    ModbusConnection();
    ~ModbusConnection();

    /**
     * @brief Configures a serial port for a modbus connection
     * 
     * @param device 
     * @param baud 
     * @param parity 
     * @param data_bit 
     * @param stop_bit 
     * @return int non-zero return if the modbus connection failed to configure
     */
    int configure(const char *device, int baud, char parity, int data_bit, int stop_bit);

    /**
     * @brief Attempt to connect using the configured settings
     * 
     * @return int non-zero return if the it failed to connect  
     */
    int connect();

    /**
     * @brief Disconnect from the modbus connection
     * 
     */
    void disconnect();

    int request(int slaveId, uint8_t* modbusRequest);

    int reply(int slaveId, uint8_t* modbusRequest, int modbusRequestResult, modbus_mapping_t* mapping);

    int readBits(int slaveId, int address, int size, uint8_t* data);
    int readInputBits(int slaveId, int address, int size, uint8_t* data);
    int readRegisters(int slaveId, int address, int size, uint16_t* data);
    int readInputRegisters(int slaveId, int address, int size, uint16_t* data);
    int writeBit(int slaveId, int address, int value);
    int writeBits(int slaveId, int address, int size, uint8_t* values);
    int writeRegister(int slaveId, int address, uint16_t value);
    int writeRegisters(int slaveId, int address, int size, uint16_t* values);
};

#endif /* MODBUSCONNECTION */
