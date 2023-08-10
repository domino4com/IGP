#ifndef IGP_H
#define IGP_H

#include <ArduinoJson.h>
#include <Wire.h>

#include "Stream.h"
#include "xIX03.h"

class IGP {
   public:
    IGP();
    bool begin();
    bool getData(uint16_t &, uint16_t &,uint16_t &,uint16_t &, uint16_t &,uint16_t &);
    bool getJSON(JsonObject& doc);
    static const uint16_t SINGLE_RESPONSE_TIME = 1000;
    static const uint16_t TOTAL_RESPONSE_TIME = 1000 * 10;
    static const uint16_t STEADY_RESPONSE_TIME = 1000 * 30;

    static const uint16_t BAUD_RATE = 9600;

    struct DATA {
        // Standard Particles, CF=1
        uint16_t PM_SP_UG_1_0;
        uint16_t PM_SP_UG_2_5;
        uint16_t PM_SP_UG_10_0;

        // Atmospheric environment
        uint16_t PM_AE_UG_1_0;
        uint16_t PM_AE_UG_2_5;
        uint16_t PM_AE_UG_10_0;
    };

    void sleep();
    void wakeUp();
    void activeMode();
    void passiveMode();
    void requestRead();
    bool read(DATA& data);
    bool readUntil(DATA& data, uint16_t timeout = SINGLE_RESPONSE_TIME);

   private:
    enum STATUS { STATUS_WAITING,
                  STATUS_OK };
    enum MODE { MODE_ACTIVE,
                MODE_PASSIVE };

    uint8_t _payload[12];
    // Stream* _stream;
    xIX03 ix;
    xIX03* _stream = &ix;
    DATA* _data;
    STATUS _status;
    MODE _mode = MODE_ACTIVE;

    uint8_t _index = 0;
    uint16_t _frameLen;
    uint16_t _checksum;
    uint16_t _calculatedChecksum;
    bool f = false;
    void loop();
};
#endif  // IGP_H