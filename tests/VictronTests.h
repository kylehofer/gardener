#ifndef VICTRON
#define VICTRON

#include <string>
#include "gtest/gtest.h"
#include "VictronSerial.h"

// Test value
#define TEST_INPUT_1 "\r\nPID	0xA053\r\nFW	159\r\nSER#	HQ21094NFGX\r\nV	22930\r\nI	-50\r\nVPV	41200\r\nPPV	8\r\nCS	3\r\nMPPT	2\r\nOR	0x00000000\r\nERR	0\r\nLOAD	ON\r\nIL	400\r\nH19	2679\r\nH20	1\r\nH21	14\r\nH22	18\r\nH23	79\r\nHSDS	297\r\nChecksum		\r\n"

/**
 * @brief Struct for holding test values
 * 
 */
typedef struct _ExpectedValues
{
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
    const char* productId;
    const char* firmware;
    const char* serial;
} ExpectedValues;

/**
 * @brief Test handler for verifying the processed results
 * 
 */
class TestHandler : VictronFieldHandler
{
private:
    ExpectedValues expectedValues;
    int count;
protected:
public:
    TestHandler() : count(0) {};
    ~TestHandler() {};
    void setExpectedValues(ExpectedValues expectedValues)
    {
        this->expectedValues = expectedValues; 
    }

    /**
     * @brief Get the number of processed fields
     * 
     * @return int 
     */
    int getCount()
    {
        return count;
    }

    /**
     * @brief Called when a field gets updated
     * 
     * @param id The id of the field
     * @param data A pointer to the fields data
     * @param size The size of the data held in memory
     */
    void fieldUpdate(uint32_t id, void* data, size_t size)
    {
        count++;
        switch (id)
        {
            case VOLTAGE:
                EXPECT_EQ(*((int32_t*) data), expectedValues.voltage);
                break;
            case PANEL_VOLTAGE:
                EXPECT_EQ(*((int32_t*) data), expectedValues.panelVoltage);
                break;
            case CURRENT:
                EXPECT_EQ(*((int16_t*) data), expectedValues.current);
                break;
            case PANEL_POWER:
                EXPECT_EQ(*((int16_t*) data), expectedValues.panelPower);
                break;
            case LOAD_CURRENT:
                EXPECT_EQ(*((int16_t*) data), expectedValues.loadCurrent);
                break;
            case YIELD_TOTAL:
                EXPECT_EQ(*((int16_t*) data), expectedValues.yieldTotal);
                break;
            case YIELD_TODAY:
                EXPECT_EQ(*((int16_t*) data), expectedValues.yieldToday);
                break;
            case MAX_POWER_TODAY:
                EXPECT_EQ(*((int16_t*) data), expectedValues.maxPowerToday);
                break;
            case YIELD_YESTERDAY:
                EXPECT_EQ(*((int16_t*) data), expectedValues.yieldYesterday);
                break;
            case MAX_POWER_YESTERDAY:
                EXPECT_EQ(*((int16_t*) data), expectedValues.maxPowerYesterday);
                break;
            case DAY_SEQUENCE:
                EXPECT_EQ(*((int16_t*) data), expectedValues.daySequence);
                break;
            case OPERATION_STATE:
                EXPECT_EQ(*((int8_t*) data), expectedValues.operationState);
                break;
            case ERROR_STATE:
                EXPECT_EQ(*((int8_t*) data), expectedValues.errorState);
                break;
            case TRACKER_OPERATION_MODE:
                EXPECT_EQ(*((int8_t*) data), expectedValues.trackerOperationMode);
                break;
            case PRODUCT_ID:
                ASSERT_STREQ((char*) data, expectedValues.productId) << "The Product ID " << ((char*) data) << " did not match " << expectedValues.productId;
                break;
            case FIRMWARE:
                ASSERT_STREQ((char*) data, expectedValues.firmware) << "The Firmware " << ((char*) data) << " did not match " << expectedValues.firmware;
                break;
            case SERIAL:
                ASSERT_STREQ((char*) data, expectedValues.serial) << "The Serial " << ((char*) data) << " did not match " << expectedValues.serial;
                break;
            case LOAD:
                EXPECT_EQ(*((bool*) data), expectedValues.load);
                break;
            case OFF_REASON:
            default:
                break;
        }
    }
};

/**
 * @brief Mock Serial to force executions for tests
 */
class MockVictronSerial : VictronSerial
{
    protected:
        using VictronSerial::doExecute;
    public:
        MockVictronSerial() : VictronSerial() {};
        MockVictronSerial(VictronFieldHandler* dataHandler) : VictronSerial(dataHandler) {};
        clock_t execute()
        {
            return doExecute();
        };
};

#endif /* VICTRON */
