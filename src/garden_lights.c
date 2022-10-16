/*
 * File: garden_lights.c
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

#include "garden_lights.h"
#include <errno.h>

#define SLAVE_ID 2
#define MAX_TIMEOUT CLOCKS_PER_SEC
#define EXECUTE_TIME CLOCKS_PER_SEC << 2
#define LIGHT_HIGH 85

enum {
    LIGHT_COMMAND,
    TOTAL_ERRORS,
    TOTAL_REGS_SIZE
};

uint16_t expected_light_command = 0;
uint16_t read_light_command = 0;

uint16_t execute_timestamp = 0;

int garden_lights_process(modbus_t *modbus_context, clock_t timestamp)
{
    if (execute_timestamp > timestamp && (timestamp - execute_timestamp) < MAX_TIMEOUT)
    {
        return 0;
    }

    int ret;
    
    int slave_id;

    ret = modbus_set_slave(modbus_context, SLAVE_ID);

    if(ret < 0) {
        return -1; // Set Slave Failed. TODO: Add DEBUG
    }

    time_t current_time = time(NULL);
    struct tm local_time = *localtime(&current_time);

    if ( // Make Time Range Configurable
        (local_time.tm_hour > 18 || (local_time.tm_hour == 18 && local_time.tm_min >= 30)) && 
        (local_time.tm_hour < 22 || (local_time.tm_hour == 22 && local_time.tm_min < 30))
        )
    {
        expected_light_command = LIGHT_HIGH;
    }
    else
    {
        expected_light_command = 0;
    }

    if (expected_light_command != read_light_command)
    {
        ret = modbus_write_register(modbus_context, LIGHT_COMMAND, expected_light_command);
        execute_timestamp = clock() + EXECUTE_TIME;
        if (ret < 0)
        {
            return -1; // Write Failed TODO: Add DEBUG
        }
    }
    else
    {
        uint16_t slave_data[TOTAL_REGS_SIZE];
        
        ret = modbus_read_registers(modbus_context, 0, TOTAL_REGS_SIZE, slave_data);
        execute_timestamp = clock() + EXECUTE_TIME;
        if (ret < 0)
        {
            return -1; // Write Failed TODO: Add DEBUG
        }
        read_light_command = slave_data[LIGHT_COMMAND];
    }

    return 0;
}