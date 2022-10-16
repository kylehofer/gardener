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

#include <stdio.h>
#include <unistd.h>
#include <modbus.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <asm/ioctls.h>
#include <stdlib.h>

#include "garden_lights.h"


int main(int argc, char *argv[])
{
    int ret = 0;
    modbus_t *modbus_context;
    clock_t timestamp;
    
    if (argc < 2)
    {
        printf("Missing Serial Port Argument\n");
        exit(EXIT_FAILURE);
    }

    struct timeval response_timeout;
    response_timeout.tv_sec = 0;
    response_timeout.tv_usec = 500000;

    modbus_context = modbus_new_rtu(argv[1], 38400, 'N', 8, 2);
    if (modbus_context == NULL)
    {
        printf("Unable to create the modbus context for the port: %s. Error: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    modbus_set_response_timeout(modbus_context, &response_timeout);

    ret = modbus_connect(modbus_context);
    if(ret < 0)
    {
        printf("Error while trying to connect to port: %s. Error: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        garden_lights_process(modbus_context, clock());
        usleep(50);
    }

    modbus_close(modbus_context);
    modbus_free(modbus_context);
}