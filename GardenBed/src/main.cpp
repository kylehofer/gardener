#include <Arduino.h>

#define USE_HOLDING_REGISTERS_ONLY

#include <Modbus.h>
#include <ModbusSerial.h>
#include <GardenBedCommon.h>

// Modbus configuration
#define MODBUS_BAUD_RATE 38400
#define MAX485_ENABLE_PIN 2

// Misc configuration
#define LIGHT_OUT_PIN 3

enum MODBUS_HOLDING_REGISTERS {
    GARDEN_LIGHT_COMMAND = MODBUS_START_REGISTER,
    TOTAL_HOLDING_REGISTERS
};

ModbusSerial modbusClient;

void setup()
{
    pinMode(LIGHT_OUT_PIN, OUTPUT);

    // Config Modbus Serial (port, speed, byte format)
    modbusClient.config(&Serial, MODBUS_BAUD_RATE, SERIAL_8N2, MAX485_ENABLE_PIN);
    // Set the Slave ID
    modbusClient.setSlaveId(MODBUS_ID);

    // Configure our holding registers (Read/Write)
    for (int i = TOTAL_HOLDING_REGISTERS - 1; i >= MODBUS_START_REGISTER; i--)
    {
        modbusClient.addHreg(i, 0);
    }

}

void loop()
{
    // Modbus main execute task. Update values etc
    modbusClient.task();

    int lightCommand = map(min(modbusClient.Hreg(GARDEN_LIGHT_COMMAND), 100), 0, 100, 0, 255);

    analogWrite(LIGHT_OUT_PIN, lightCommand);

    // Using delay here as it's a fairly simple sketch
    delay(200);
}
