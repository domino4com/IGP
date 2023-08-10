/*!
 * @file IGP.cpp
 * @brief Get data from IGP
 * @n ...
 * @copyright   MIT License
 * @author [Bjarke Gotfredsen](bjarke@gotfredsen.com)
 * @version  V1.0
 * @date  2023
 * @https://github.com/domino4com/IGP
 */
#include "IGP.h"

IGP::IGP() {
}

bool IGP::begin() {
    _stream->begin(9600);
    for (uint8_t i = 0; i < 4; i++) {
        _stream->pinMode(i, 1);
        _stream->digitalWrite(i, 1);
    }
    wakeUp();
    return true;
}

bool IGP::getData(uint16_t &pm1a, uint16_t &pm25a, uint16_t &pm10a, uint16_t &pm1s, uint16_t &pm25s, uint16_t &pm10s) {
    DATA data;
    while (!read(data)) {
        delay(1);
    }
    pm1a = data.PM_AE_UG_1_0;
    pm25a = data.PM_AE_UG_2_5;
    pm10a = data.PM_AE_UG_10_0;
    pm1s = data.PM_SP_UG_1_0;
    pm25s = data.PM_SP_UG_2_5;
    pm10s = data.PM_SP_UG_10_0;

    return true;  // Return true for successful read (add error handling if needed)
}

bool IGP::getJSON(JsonObject &doc) {
    uint16_t pm1a, pm25a, pm10a, pm1s, pm25s, pm10s;
    if (!getData(pm1a, pm25a, pm10a, pm1s, pm25s, pm10s)) {
        return false;
    }

    JsonArray dataArray = doc.createNestedArray("IGP");

    JsonObject dataSet = dataArray.createNestedObject();  // First data set
    dataSet["name"] = "PM1 Atm";
    dataSet["value"] = pm1a;
    dataSet["unit"] = "µg/m3";

    dataArray.add(dataSet);  // Subsequent data sets
    dataSet["name"] = "PM2.5 Atm";
    dataSet["value"] = pm25a;
    dataSet["unit"] = "µg/m3";

    dataArray.add(dataSet);  // Subsequent data sets
    dataSet["name"] = "PM10 Atm";
    dataSet["value"] = pm10a;
    dataSet["unit"] = "µg/m3";

    dataArray.add(dataSet);  // Subsequent data sets
    dataSet["name"] = "PM1 Std";
    dataSet["value"] = pm1s;
    dataSet["unit"] = "µg/m3";

    dataArray.add(dataSet);  // Subsequent data sets
    dataSet["name"] = "PM2.5 Std";
    dataSet["value"] = pm25s;
    dataSet["unit"] = "µg/m3";

    dataArray.add(dataSet);  // Subsequent data sets
    dataSet["name"] = "PM10 Std";
    dataSet["value"] = pm10s;
    dataSet["unit"] = "µg/m3";

    return true;
}

// Standby mode. For low power consumption and prolong the life of the sensor.
void IGP::sleep() {
    uint8_t command[] = {0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
    _stream->write(command, sizeof(command));
}

// Operating mode. Stable data should be got at least 30 seconds after the sensor wakeup from the sleep mode because of the fan's performance.
void IGP::wakeUp() {
    uint8_t command[] = {0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
    _stream->write(command, sizeof(command));
}

// Active mode. Default mode after power up. In this mode sensor would send serial data to the host automatically.
void IGP::activeMode() {
    uint8_t command[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
    _stream->write(command, sizeof(command));
    _mode = MODE_ACTIVE;
}

// Passive mode. In this mode sensor would send serial data to the host only for request.
void IGP::passiveMode() {
    uint8_t command[] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
    _stream->write(command, sizeof(command));
    _mode = MODE_PASSIVE;
}

// Request read in Passive Mode.
void IGP::requestRead() {
    if (_mode == MODE_PASSIVE) {
        uint8_t command[] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};
        _stream->write(command, sizeof(command));
    }
}

// Non-blocking function for parse response.
bool IGP::read(DATA &data) {
    _data = &data;
    loop();

    return _status == STATUS_OK;
}

// Blocking function for parse response. Default timeout is 1s.
bool IGP::readUntil(DATA &data, uint16_t timeout) {
    _data = &data;
    uint32_t start = millis();
    do {
        loop();
        if (_status == STATUS_OK) break;
    } while (millis() - start < timeout);
    return _status == STATUS_OK;
}

void IGP::loop() {
    _status = STATUS_WAITING;
    if (_stream->available()) {
        uint8_t ch = _stream->read();

        switch (_index) {
            case 0:
                if (ch != 0x42) {
                    return;
                }
                _calculatedChecksum = ch;
                break;

            case 1:
                if (ch != 0x4D) {
                    _index = 0;
                    return;
                }
                _calculatedChecksum += ch;
                break;

            case 2:
                _calculatedChecksum += ch;
                _frameLen = ch << 8;
                break;

            case 3:
                _frameLen |= ch;
                // Unsupported sensor, different frame length, transmission error e.t.c.
                if (_frameLen != 2 * 9 + 2 && _frameLen != 2 * 13 + 2) {
                    _index = 0;
                    return;
                }
                _calculatedChecksum += ch;
                break;

            default:
                if (_index == _frameLen + 2) {
                    _checksum = ch << 8;
                } else if (_index == _frameLen + 2 + 1) {
                    _checksum |= ch;

                    if (_calculatedChecksum == _checksum) {
                        _status = STATUS_OK;

                        // Standard Particles, CF=1.
                        _data->PM_SP_UG_1_0 = makeWord(_payload[0], _payload[1]);
                        _data->PM_SP_UG_2_5 = makeWord(_payload[2], _payload[3]);
                        _data->PM_SP_UG_10_0 = makeWord(_payload[4], _payload[5]);

                        // Atmospheric Environment.
                        _data->PM_AE_UG_1_0 = makeWord(_payload[6], _payload[7]);
                        _data->PM_AE_UG_2_5 = makeWord(_payload[8], _payload[9]);
                        _data->PM_AE_UG_10_0 = makeWord(_payload[10], _payload[11]);
                    }

                    _index = 0;
                    return;
                } else {
                    _calculatedChecksum += ch;
                    uint8_t payloadIndex = _index - 4;

                    // Payload is common to all sensors (first 2x6 bytes).
                    if (payloadIndex < sizeof(_payload)) {
                        _payload[payloadIndex] = ch;
                    }
                }

                break;
        }

        _index++;
    }
}