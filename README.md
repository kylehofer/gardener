# Gardener - Garden Control System
This project is for managing my automated gardening system. The system is powered from a few AGM batteries hooked up to an 80W solar system. The end goal for this system is to manage garden lighting, watering, and data collecting. The Gardener is running on a Beaglebone Black, that is interfaced with a control board containing Serial Ports (UART and RS485), and a set of IOs. Communication to and from the Gardener will be handled with MQTT, with the end goal of adding support for Sparkplug. The Gardener will talk to devices in the field using Modbus RTU.

## Hardware Goals
- [x] Garden Light Module
- [ ] Watering Module
- [ ] Weather Module
- [ ] WiFi Support

## Software Goals

- [x] Modbus Support
- [x] Garden Light Module Communication
- [x] Serial Communication with Victron Smart Solar
- [x] Test Framework Added
- [ ] MQTT Support
- [ ] Remote Configuration
- [ ] Remote Commands
- [ ] Sparkplug Support
- [ ] Smart Scheduling (Sunrise/Sunset etc)