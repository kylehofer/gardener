/*
 * File: GardenShedCommon.h
 * Project: gardener
 * Created Date: Saturday November 12th 2022
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

#ifndef GARDENSHEDCOMMON
#define GARDENSHEDCOMMON

#include "ModbusUtils.h"

// Starting register id
#define MODBUS_START_REGISTER 0
#define MODBUS_ID 3

enum MODBUS_INPUT_REGISTERS {
    DOUBLE_REGISTER_VALUE(VICTRON_VOLTAGE, MODBUS_START_REGISTER),
    DOUBLE_REGISTER(VICTRON_PANEL_VOLTAGE),
    VICTRON_CURRENT,
    VICTRON_PANEL_POWER,
    VICTRON_LOAD_CURRENT,
    VICTRON_OPERATION_STATE,
    VICTRON_ERROR_STATE,
    VICTRON_LOAD,
    VICTRON_YIELD_TOTAL,
    VICTRON_YIELD_TODAY,
    VICTRON_MAX_POWER_TODAY,
    VICTRON_YIELD_YESTERDAY,
    VICTRON_MAX_POWER_YESTERDAY,
    VICTRON_TRACKER_OPERATION_MODE,
    VICTRON_DAY_SEQUENCE,
    VICTRON_SERIAL_NUMBER,
    VICTRON_PRODUCT_ID,
    VICTRON_FIRMWARE,
    VICTRON_OFF_REASON,
    TOTAL_INPUT_REGISTERS
};

enum MODBUS_HOLDING_REGISTERS {
    SHED_LIGHT_COMMAND = MODBUS_START_REGISTER,
    TOTAL_HOLDING_REGISTERS
};

#endif /* GARDENSHEDCOMMON */
