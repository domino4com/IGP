#include <ArduinoJson.h>
#include <Wire.h>
#ifndef I2C_SDA
#define I2C_SDA SDA
#endif
#ifndef I2C_SCL
#define I2C_SCL SCL
#endif

// Specifics
#include <IGP.h>
IGP input;
uint16_t pm1a, pm25a, pm10a, pm1s, pm25s, pm10s;
char s[] = "Atmospheric: PM1:%u, PM2.5:%u, PM10:%u\nStandard:    PM1:%u, PM2.5:%u, PM10:%u\n";

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.printf("\nIGP Example Test\n");

    Wire.setPins(I2C_SDA, I2C_SCL);
    Wire.begin();

    if (input.begin()) {
        Serial.println("IGP initialized successfully.");
    } else {
        Serial.println("Failed to initialize IGP!");
        exit(0);
    }
}

void loop() {
    if (input.getData(pm1a, pm25a, pm10a, pm1s, pm25s, pm10s)) {
        Serial.printf(s, pm1a, pm25a, pm10a, pm1s, pm25s, pm10s);
    } else {
        Serial.println("Failed to get IGP data.");
    }

    delay(1000);
}