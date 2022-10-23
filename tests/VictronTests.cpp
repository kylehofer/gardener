#include "gtest/gtest.h"

#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <signal.h>
#include <termios.h>

#include "VictronTests.h"

#include "utils.h"

#define TIMEOUT 10000

const std::string INPUT_PORT = "./input";
const std::string OUTPUT_PORT = "./output";
const std::string PORT_COMMAND = "socat pty,raw,echo=0,link=" + INPUT_PORT + " pty,raw,echo=0,link=" + OUTPUT_PORT;

#define VICTRON_PORT "./output"
#define VICTRON_BAUD B19200
#define VICTRON_DATA_BITS 8
#define VICTRON_STOP_BITS 1
#define VICTRON_PARITY 'N' 

/**
 * @brief Checked whether or not a file exists
 * 
 * @param name 
 * @return true 
 * @return false 
 */
inline bool fileExists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

TEST(VictronSerial, TestSerialPayload) {
    int timeoutCount = 0;
    int inPort;
    TestHandler* handler;
    VictronSerial victronSerial;

    // Start up virtual ports on a seperate thread
    std::thread portThread(&system, PORT_COMMAND.c_str());

    // Wait till out input/output ports are open
    while (!fileExists(INPUT_PORT) && !fileExists(OUTPUT_PORT) && timeoutCount++ < TIMEOUT)
    {
        usleep(50);
    }
    
    usleep(1);

    if (timeoutCount >= TIMEOUT)
    {
        system("killall socat");
    }

    handler = new TestHandler();
    victronSerial = VictronSerial((VictronDataHandler *) handler);

    if (victronSerial.initialize(OUTPUT_PORT.c_str(), VICTRON_BAUD, VICTRON_PARITY, VICTRON_DATA_BITS, VICTRON_STOP_BITS))
    {
        system("killall socat");
        exit(EXIT_FAILURE);
    }

    inPort = open_port(INPUT_PORT.c_str(), VICTRON_BAUD, VICTRON_PARITY, VICTRON_DATA_BITS, VICTRON_STOP_BITS);

    if (set_port_synchronization(OUTPUT_PORT.c_str(), 0, 10) < 0)
    {
        system("killall socat");
        exit(EXIT_FAILURE);
    }

    // read entire file into string
    if(std::ifstream is{"./test_input", std::ios::binary | std::ios::ate}) {
        auto size = is.tellg();
        std::string str(size, '\0'); // construct string to stream size
        is.seekg(0);
        if(is.read(&str[0], size))
        {
            std::cout << str << '\n';
            write(inPort, str.c_str(), size);
        } 
    }

    handler->setExpectedValues({
        .voltage = 22930,
        .panelVoltage = 41200,
        .current = -50,
        .panelPower = 8,
        .loadCurrent = 400,
        .yieldTotal = 2679,
        .yieldToday = 1,
        .maxPowerToday = 14,
        .yieldYesterday = 18,
        .maxPowerYesterday = 79,
        .daySequence = 297,
        .operationState = 3,
        .errorState = 0,
        .trackerOperationMode = 2,
        .load = 1,
        .productId = "0xA053",
        .firmware = "159",
        .serial = "HQ21094NFGX",
    });

    victronSerial.execute();

    EXPECT_EQ(handler->didProcess(), 1);

    system("killall socat");

    portThread.join();

    delete handler;
}
