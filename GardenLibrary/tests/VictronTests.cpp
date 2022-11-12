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

TEST(VictronSerial, TestClassHandlerPayload) {
    TestHandler* handler;
    VictronParser victronSerial;

    handler = new TestHandler();
    victronSerial = VictronParser((VictronFieldHandler *) handler);

    
    // Setting out test values
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

    victronSerial.parse(TEST_INPUT_1, sizeof(TEST_INPUT_1));

    // Matching the number of fields processed matches
    EXPECT_EQ(handler->getCount(), 18);
}