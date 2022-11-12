#include <SoftwareSerial.h>
#include <Arduino.h>
#include <VictronParser.h>

#include <GardenShedCommon.h>

#include <Modbus.h>
#include <ModbusSerial.h>

// Serial configuration
#define SERIAL_TIMER 500UL
#define MAX_SERIAL_TIMER 1000UL
#define SERIAL_BUFFER_SIZE 256
#define SERIAL_BUFFER_MIN 1
#define SOFTWARE_SERIAL_TX 12
#define SOFTWARE_SERIAL_RX 13

// Modbus configuration
#define MODBUS_BAUD_RATE 38400
#define MAX485_ENABLE_PIN 2

// Misc configuration
#define LIGHT_OUT_PIN 5
#define DOOR_SENSOR_PIN 10
#define DEBOUNCE_TIMER 5

enum DoorState
{
    OPEN,
    CLOSED
};


ModbusSerial modbusClient;

/**
 * @brief Called for each field when the VictronParser recieves a valid transmission
 * 
 * @param id The id of the field
 * @param data A pointer to the raw data of the field
 * @param size The data size of the raw data
 */
void victronDataHandler(uint32_t id, void* data, size_t size)
{
    switch (id)
    {
        case VOLTAGE:
            WRITE_DOUBLE_REGISTER(modbusClient.Ireg, VICTRON_VOLTAGE, *((int32_t*) data));
            break;
        case PANEL_VOLTAGE:
            WRITE_DOUBLE_REGISTER(modbusClient.Ireg, VICTRON_PANEL_VOLTAGE, *((int32_t*) data));
            break;
        case CURRENT:
            modbusClient.Ireg(VICTRON_CURRENT, *((int16_t*) data));
            break;    
        case YIELD_TODAY:
            modbusClient.Ireg(VICTRON_YIELD_TOTAL, *((int16_t*) data));
            break;
        case MAX_POWER_TODAY:
            modbusClient.Ireg(VICTRON_MAX_POWER_TODAY, *((int16_t*) data));
            break;
        case YIELD_YESTERDAY:
            modbusClient.Ireg(VICTRON_YIELD_YESTERDAY, *((int16_t*) data));
            break;
        case MAX_POWER_YESTERDAY:
            modbusClient.Ireg(VICTRON_MAX_POWER_YESTERDAY, *((int16_t*) data));
            break;
        case DAY_SEQUENCE:
            modbusClient.Ireg(VICTRON_DAY_SEQUENCE, *((int16_t*) data));
            break;
        case OPERATION_STATE:
            modbusClient.Ireg(VICTRON_OPERATION_STATE, *((int8_t*) data));
            break;
        case ERROR_STATE:
            modbusClient.Ireg(VICTRON_ERROR_STATE, *((int8_t*) data));
            break;
        case TRACKER_OPERATION_MODE:
            modbusClient.Ireg(VICTRON_TRACKER_OPERATION_MODE, *((int8_t*) data));
            break;
        case LOAD:
            modbusClient.Ireg(VICTRON_LOAD, *((bool*) data));
            break;
        case PRODUCT_ID: // All Strings, Might implement in Modbus later
        case FIRMWARE:
        case SERIAL_NUMBER:
        case OFF_REASON:
        default:
            break;
    }
};

VictronParser victronParser = VictronParser(victronDataHandler);
SoftwareSerial softwareSerial = SoftwareSerial(SOFTWARE_SERIAL_RX, SOFTWARE_SERIAL_TX);

void setup()
{
    pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
    pinMode(LIGHT_OUT_PIN, OUTPUT);

    // Config Modbus Serial (port, speed, byte format)
    modbusClient.config(&Serial, MODBUS_BAUD_RATE, SERIAL_8N2, MAX485_ENABLE_PIN);
    // Set the Slave ID
    modbusClient.setSlaveId(MODBUS_ID);

    // Configure our input registers (Read only)
    for (int i = TOTAL_INPUT_REGISTERS - 1; i >= MODBUS_START_REGISTER; i--)
    {
        modbusClient.addIreg(i);
    }

    // Configure our holding registers (Read/Write)
    for (int i = TOTAL_HOLDING_REGISTERS - 1; i >= MODBUS_START_REGISTER; i--)
    {
        modbusClient.addHreg(i);
    }
}

/**
 * @brief Used for calculating the state of the door and whether or not to enable the light
 * 
 */
inline void doorHandler()
{
    // Used for calculating a debounce timer
    static DoorState doorState = CLOSED;
    static int debounce = 0;
    
    // lambda to make common functionality have the above scoped variables
    static auto debounceCheck = [&] (DoorState next) {
        if (doorState != next)
        {
            if (debounce >= DEBOUNCE_TIMER)
            {
                doorState = next;
                debounce = 0;
                return true;
            }
            debounce++;
            return false;
        }
        debounce = 0;
        return false;
    };

    // Button is pressed if digitalRead returns 0
    if (digitalRead(DOOR_SENSOR_PIN) == 1)
    {
        if (debounceCheck(OPEN)) 
        {
            int lightCommand = map(min(modbusClient.Hreg(SHED_LIGHT_COMMAND), 100), 0, 100, 0, 255);
            analogWrite(LIGHT_OUT_PIN, lightCommand);
        }
    }
    else
    {
        if (debounceCheck(CLOSED)) 
        {
            analogWrite(LIGHT_OUT_PIN, 0);
        }
    }
}

/**
 * @brief Reads from the victron serial and handles 
 * 
 */
inline void victronHandler()
{
    // Buffer for reading from the serial line
    static char buffer[SERIAL_BUFFER_SIZE];
    // Timer so we don't just constantly execute
    static unsigned long timestamp = millis();

    // Calculate difference in time since last serial read
    unsigned long difference = millis() - timestamp;

    // Either enough time has passed, or the millis has wrapped around
    if (difference > SERIAL_TIMER || difference > MAX_SERIAL_TIMER)
    {
        // Read buffer until nothing left
        while(softwareSerial.available() > SERIAL_BUFFER_MIN)
        {
            int bytes = softwareSerial.readBytes(buffer, SERIAL_BUFFER_SIZE);
            victronParser.parse(buffer, bytes);
        }
    }
}

void loop()
{
    // Modbus main execute task. Update values etc
    modbusClient.task();
    doorHandler();
    victronHandler();
}
