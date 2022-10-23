#ifndef VICTRON
#define VICTRON

#include <string>
#include "gtest/gtest.h"
#include "VictronSerial.h"

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
                    EXPECT_EQ(*((int32_t*)field->getData()), expectedValues.voltage);
                    break;
                case PANEL_VOLTAGE:
                    EXPECT_EQ(*((int32_t*)field->getData()), expectedValues.panelVoltage);
                    break;
                case CURRENT:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.current);
                    break;
                case PANEL_POWER:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.panelPower);
                    break;
                case LOAD_CURRENT:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.loadCurrent);
                    break;
                case YIELD_TOTAL:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.yieldTotal);
                    break;
                case YIELD_TODAY:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.yieldToday);
                    break;
                case MAX_POWER_TODAY:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.maxPowerToday);
                    break;
                case YIELD_YESTERDAY:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.yieldYesterday);
                    break;
                case MAX_POWER_YESTERDAY:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.maxPowerYesterday);
                    break;
                case DAY_SEQUENCE:
                    EXPECT_EQ(*((int16_t*)field->getData()), expectedValues.daySequence);
                    break;
                case OPERATION_STATE:
                    EXPECT_EQ(*((int8_t*)field->getData()), expectedValues.operationState);
                    break;
                case ERROR_STATE:
                    EXPECT_EQ(*((int8_t*)field->getData()), expectedValues.errorState);
                    break;
                case TRACKER_OPERATION_MODE:
                    EXPECT_EQ(*((int8_t*)field->getData()), expectedValues.trackerOperationMode);
                    break;
                case PRODUCT_ID:
                    EXPECT_EQ(*((std::string*)field->getData()), expectedValues.productId);
                    break;
                case FIRMWARE:
                    EXPECT_EQ(*((std::string*)field->getData()), expectedValues.firmware);
                    break;
                case SERIAL:
                    EXPECT_EQ(*((std::string*)field->getData()), expectedValues.serial);
                    break;
                case LOAD: // ON/OFF value
                    EXPECT_EQ(*((int8_t*)field->getData()), expectedValues.load);
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

#endif /* VICTRON */
