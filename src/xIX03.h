#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <inttypes.h>

#if defined(ESP32) || defined(ESP8266)
#include <stdlib_noniso.h>
#else
#include <avr/dtostrf.h>
#endif

#define SC16IS740_RHR 0x00
#define SC16IS740_THR 0x00
#define SC16IS740_IER 0x01
#define SC16IS740_FCR 0x02
#define SC16IS740_IIR 0x02
#define SC16IS740_LCR 0x03
#define SC16IS740_MCR 0x04
#define SC16IS740_LSR 0x05
#define SC16IS740_MSR 0x06
#define SC16IS740_SPR 0x07
#define SC16IS740_TCR 0x06
#define SC16IS740_TLR 0x07
#define SC16IS740_TXLVL 0x08
#define SC16IS740_RXLVL 0x09
#define SC16IS740_IODIR 0x0A
#define SC16IS740_IOSTATE 0x0B
#define SC16IS740_IOINTENA 0x0C
#define SC16IS740_RESERVED 0x0D
#define SC16IS740_IOCONTROL 0x0E
#define SC16IS740_EFCR 0x0F
#define SC16IS740_DLL 0x00
#define SC16IS740_DLH 0x01
#define SC16IS740_EFR 0x02
#define SC16IS740_XON1 0x04
#define SC16IS740_XON2 0x05
#define SC16IS740_XOFF1 0x06
#define SC16IS740_XOFF2 0x07

#define SC16IS740_I2C_ADDR 0x4D

class xIX03
{
public:
    xIX03();
    xIX03(uint8_t addr);
    bool begin(uint16_t baudRate);
    uint8_t available();
    uint8_t availableForWrite();
    void end();
    // bool find();
    // findUntil();
    void flush();
    // parseFloat();
    // parseInt();
    // peek();
    void print(const String &);
    void print(const char[]);
    void print(char);
    void print(unsigned char, int = DEC);
    void print(int, int = DEC);
    void print(unsigned int, int = DEC);
    void print(long, int = DEC);
    void print(unsigned long, int = DEC);
    void print(double, int = 2);

    void println(const String &s);
    void println(const char[]);
    void println(char);
    void println(unsigned char, int = DEC);
    void println(int, int = DEC);
    void println(unsigned int, int = DEC);
    void println(long, int = DEC);
    void println(unsigned long, int = DEC);
    void println(double, int = 2);
    void println(void);
    void config();
    uint8_t read();
    // readBytes();
    // readBytesUntil();
    // readString();
    // readStringUntil();
    // setTimeout();
    // serialEvent();

    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t state);
    bool digitalRead(uint8_t pin);

    void write(uint8_t);
    void write(const char *str);
    void write(const uint8_t *buffer, uint8_t size);
    void write(const char *buffer, uint8_t size);

private:
    bool readLSR();
    void setBaudRate(uint16_t baudRate);
    void resetDevice();
    bool ping();
    void FIFOEnable(uint8_t fifo_enable);
    uint8_t i2cAddr;
    void writeByte(uint8_t reg, uint8_t val);
    uint8_t readByte(uint8_t reg);
    void writeBlock(uint8_t reg, uint8_t *val, uint8_t len);
};