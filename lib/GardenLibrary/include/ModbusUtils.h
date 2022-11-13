/*
 * File: ModbusUtils.h
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

#ifndef MODBUSUTILS
#define MODBUSUTILS

// Utility macros for handling 32 bit values with modbus
// Splits a single identifier into UPPER and LOWER parts
#define DOUBLE_REGISTER(register) register##_UPPER, register##_LOWER
#define DOUBLE_REGISTER_VALUE(register, value) register##_UPPER = value, register##_LOWER
// Writes a single 32 bit value into two 16 bit modbus registers
#define WRITE_DOUBLE_REGISTER(func, register, value) func(register##_UPPER, ((value) >> 16) & 0xFFFF); func(register##_UPPER, (value) & 0xFFFF);

#endif /* MODBUSUTILS */
