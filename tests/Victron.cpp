#include "gtest/gtest.h"
#include <string>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <signal.h>
#include <termios.h>

#include "VictronSerial.h"
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

typedef struct _ExpectedValues
{
    int32_t test32int;
    int16_t test16int;
    int8_t test8int;
    std::string testString;

    int32_t voltage;
    int32_t panelVoltage;
    int16_t current;
    int16_t panelPower;
    int16_t loadCurrent;
    int16_t yieldTotal;
    int16_t yieldToday;
    int16_t maxPowerToday;
    int16_t yieldYesterday;
    int16_t maxPowerYesterday;
    int16_t daySequence;
    int8_t operationState;
    int8_t errorState;
    int8_t trackerOperationMode;
    int8_t load;
    std::string productId;
    std::string firmware;
    std::string serial;
} ExpectedValues;

class TestHandler : VictronDataHandler
{
private:
    ExpectedValues expectedValues;
    bool processed;
protected:
public:
    TestHandler() : processed(0) {};
    ~TestHandler() {};
    void setExpectedValues(ExpectedValues expectedValues)
    {
        this->expectedValues = expectedValues; 
    }

    bool didProcess()
    {
        return processed;
    }

    void fieldsUpdate(std::queue<Field*>* fields)
    {
        processed = true;
        Field* field;
        // Clear Queue
        while (!fields->empty()) 
        {
            field = fields->front();
            fields->pop();

            switch (field->getId())
            {
                case VOLTAGE:
                    field->getData(&expectedValues.test32int);
                    EXPECT_EQ(expectedValues.test32int, expectedValues.voltage);
                    break;
                case PANEL_VOLTAGE:
                    field->getData(&expectedValues.test32int);
                    EXPECT_EQ(expectedValues.test32int, expectedValues.panelVoltage);
                    break;
                case CURRENT:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.current);
                    break;
                case PANEL_POWER:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.panelPower);
                    break;
                case LOAD_CURRENT:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.loadCurrent);
                    break;
                case YIELD_TOTAL:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.yieldTotal);
                    break;
                case YIELD_TODAY:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.yieldToday);
                    break;
                case MAX_POWER_TODAY:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.maxPowerToday);
                    break;
                case YIELD_YESTERDAY:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.yieldYesterday);
                    break;
                case MAX_POWER_YESTERDAY:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.maxPowerYesterday);
                    break;
                case DAY_SEQUENCE:
                    field->getData(&expectedValues.test16int);
                    EXPECT_EQ(expectedValues.test16int, expectedValues.daySequence);
                    break;
                case OPERATION_STATE:
                    field->getData(&expectedValues.test8int);
                    EXPECT_EQ(expectedValues.test8int, expectedValues.operationState);
                    break;
                case ERROR_STATE:
                    field->getData(&expectedValues.test8int);
                    EXPECT_EQ(expectedValues.test8int, expectedValues.errorState);
                    break;
                case TRACKER_OPERATION_MODE:
                    field->getData(&expectedValues.test8int);
                    EXPECT_EQ(expectedValues.test8int, expectedValues.trackerOperationMode);
                    break;
                case PRODUCT_ID:
                    field->getData(&expectedValues.testString);
                    EXPECT_EQ(expectedValues.testString, expectedValues.productId);
                    break;
                case FIRMWARE:
                    field->getData(&expectedValues.firmware);
                    EXPECT_EQ(expectedValues.testString, expectedValues.productId);
                    break;
                case SERIAL:
                    field->getData(&expectedValues.testString);
                    EXPECT_EQ(expectedValues.testString, expectedValues.serial);
                    break;
                case LOAD: // ON/OFF value
                    field->getData(&expectedValues.test8int);
                    EXPECT_EQ(expectedValues.test8int, expectedValues.load);
                    break;
                case OFF_REASON:
                default:
                    break;
            }
            delete field;
        }
    
    };
};

class MockVictronSerial : VictronSerial
{
    protected:
        using VictronSerial::doExecute;
    public:
        MockVictronSerial() : VictronSerial() {};
        MockVictronSerial(VictronDataHandler* dataHandler) : VictronSerial(dataHandler) {};
        clock_t execute()
        {
            return doExecute();
        };
};

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
    std::thread portThread(&system, PORT_COMMAND.c_str());

    TestHandler* handler = new TestHandler();

    while (!fileExists(INPUT_PORT) && !fileExists(OUTPUT_PORT) && timeoutCount++ < TIMEOUT)
    {
        usleep(50);
    }
    
    usleep(1);

    if (timeoutCount >= TIMEOUT)
    {
        system("killall socat");
    }

    VictronSerial victronSerial = VictronSerial((VictronDataHandler *) handler);

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
