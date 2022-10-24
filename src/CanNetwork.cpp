/*
 * File: CanNetwork.cpp
 * Project: gardener
 * Created Date: Sunday October 23rd 2022
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

#include "CanNetwork.h"

// C++ libs
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>
// C Libs
#include <unistd.h>

// Socket Libs
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

// Can Libs
#include <linux/can.h>
#include <linux/can/raw.h>

#define CAN_ADDRESS "can0"

CanNetwork::CanNetwork() : initialized(false) {};
CanNetwork::~CanNetwork() 
{
    closeSocket();
};

int CanNetwork::initialize()
{
    struct sockaddr_can canAddress;
    struct ifreq interfaceRequest;

    if ((socketId = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return 1;
    }

    strcpy(interfaceRequest.ifr_name, CAN_ADDRESS);
    ioctl(socketId, SIOCGIFINDEX, &interfaceRequest);

    memset(&canAddress, 0, sizeof(canAddress));
    canAddress.can_family = AF_CAN;
    canAddress.can_ifindex = interfaceRequest.ifr_ifindex;

    if (bind(socketId, (struct sockaddr *)&canAddress, sizeof(canAddress)) < 0) {
        perror("Bind");
        return 1;
    }

    struct can_filter canFilter[1];

    canFilter[0].can_id   = 0x0;
    canFilter[0].can_mask = 0x0;
    //canFilter[1].can_id   = 0x200;
    //canFilter[1].can_mask = 0x700;

    setsockopt(socketId, SOL_CAN_RAW, CAN_RAW_FILTER, &canFilter, sizeof(canFilter));
    return 0;
}

int CanNetwork::readCan()
{
    int count, i;
    struct can_frame frame;

    count = read(socketId, &frame, sizeof(struct can_frame));

    if (count < 0)
    {
        std::cout << "Error while reading\n";
        return 1;
    }

    printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

    for (i = 0; i < frame.can_dlc; i++)
    {
        printf("%02X ",frame.data[i]);
    }

    printf("\r\n");
    return 0;
}

int CanNetwork::closeSocket()
{
    if (initialized && close(socketId) < 0)
    {
        std::cout << "Error while closing\n";
        return 1;
    }

    return 0;
}

clock_t CanNetwork::doExecute()
{
    readCan();
    return 0;
}