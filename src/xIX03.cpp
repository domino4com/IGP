#include "xIX03.h"

xIX03::xIX03()
{
    i2cAddr = SC16IS740_I2C_ADDR;
}

xIX03::xIX03(uint8_t addr)
{
    i2cAddr = addr;
}

bool xIX03::begin(uint16_t baudRate)
{
    Wire.beginTransmission(i2cAddr);
    if (Wire.endTransmission() != 0)
        return false;
    if (!ping())
        return false;

    resetDevice();
    FIFOEnable(1);
    setBaudRate(baudRate);
    config();
    return true;
}

void xIX03::config()
{
    writeByte(SC16IS740_LCR, 0x03); // SERIAL_8N1
}

void xIX03::setBaudRate(uint16_t baudRate)
{
    uint8_t old_data, new_data;
    uint16_t divisor = (uint16_t)((1.8432 * 1000000UL) / (baudRate * 16));
    uint8_t baud_low = (uint8_t)(divisor);
    uint8_t baud_hi = (uint8_t)(divisor >> 8);
    old_data = readByte(SC16IS740_LCR);
    new_data |= 0x80;
    writeByte(SC16IS740_LCR, new_data);
    writeByte(SC16IS740_DLL, baud_low);
    writeByte(SC16IS740_DLH, baud_hi);
    writeByte(SC16IS740_LCR, 0x7F);
}

void xIX03::resetDevice()
{
    uint8_t reg;
    reg = readByte(SC16IS740_IOCONTROL);
    reg |= 0x08;
    writeByte(SC16IS740_IOCONTROL, reg);
}

bool xIX03::ping()
{
    writeByte(SC16IS740_SPR, 0x99);
    if (readByte(SC16IS740_SPR) != 0x99)
        return false;
    return true;
}

void xIX03::FIFOEnable(uint8_t fifo_enable)
{
    uint8_t temp_fcr = readByte(SC16IS740_FCR);
    if (fifo_enable == 0)
        temp_fcr &= ~(1 << 1);
    else
        temp_fcr |= 1 << 1;
    writeByte(SC16IS740_FCR, temp_fcr);
    return;
}

bool xIX03::readLSR()
{
    uint8_t tmp_lsr;
    do
    {
        tmp_lsr = readByte(SC16IS740_LSR);
    } while ((tmp_lsr & 0x20) == 0);
    return true;
}

void xIX03::print(const String &s)
{
    const char *conv = s.c_str();
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)conv, strlen(s.c_str()));
}

void xIX03::print(const char str[])
{
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)str, strlen(str));
}

void xIX03::print(char c)
{
    if (readLSR())
        writeByte(SC16IS740_THR, (uint8_t)c);
}

void xIX03::print(unsigned char b, int base)
{
    if (readLSR())
        writeByte(SC16IS740_THR, (uint8_t)b);
}

void xIX03::print(int n, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)n;
    val[1] = uint8_t(n >> 8);
    val[2] = uint8_t(n >> 16);
    val[3] = uint8_t(n >> 24);
    if (readLSR())
        writeBlock(SC16IS740_THR, val, 4);
}

void xIX03::print(unsigned int n, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)n;
    val[1] = uint8_t(n >> 8);
    val[2] = uint8_t(n >> 16);
    val[3] = uint8_t(n >> 24);
    if (readLSR())
        writeBlock(SC16IS740_THR, val, 4);
}

void xIX03::print(long n, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)n;
    val[1] = uint8_t(n >> 8);
    val[2] = uint8_t(n >> 16);
    val[3] = uint8_t(n >> 24);
    if (readLSR())
        writeBlock(SC16IS740_THR, val, 4);
}

void xIX03::print(unsigned long n, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)n;
    val[1] = uint8_t(n >> 8);
    val[2] = uint8_t(n >> 16);
    val[3] = uint8_t(n >> 24);
    if (readLSR())
        writeBlock(SC16IS740_THR, val, 4);
}

void xIX03::print(double n, int digits)
{
    String _s = String(n);
    uint8_t i = sizeof(_s);
    char c[i + digits];
    dtostrf(n, i, digits, c);
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)c, i + digits);
}

void xIX03::println(void)
{
    char newline[1] = {'\n'};
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)newline, 1);
}

// void xIX03::println(const String &s)
// {
//     String _s = s + "\r\n";
//     uint8_t i = strlen(_s.c_str());
//     char final[i] = {\0};
//     const char *conv = _s.c_str();
//     strcat(final, conv);
//     if (readLSR())
//         writeBlock(SC16IS740_THR, (uint8_t *)conv, sizeof(final));
// }

// void xIX03::println(const char c[])
// {
//     String _s = String(c) + "\r\n";
//     uint8_t i = strlen(_s.c_str());
//     char final[i] = {\0};
//     const char *conv = _s.c_str();
//     strcat(final, conv);
//     if (readLSR())
//         writeBlock(SC16IS740_THR, (uint8_t *)conv, sizeof(final));
// }

void xIX03::println(char c)
{
    String _s = String(c) + "\r\n";
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)_s.c_str(), 3);
}

void xIX03::println(unsigned char b, int base)
{
    String _s = String(b) + "\r\n";
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)_s.c_str(), 3);
}

void xIX03::println(int num, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)num;
    val[1] = uint8_t(num >> 8);
    val[2] = uint8_t(num >> 16);
    val[3] = uint8_t(num >> 24);
    String _s = String((char *)val) + "\r\n";
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)_s.c_str(), 6);
}

void xIX03::println(unsigned int num, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)num;
    val[1] = uint8_t(num >> 8);
    val[2] = uint8_t(num >> 16);
    val[3] = uint8_t(num >> 24);
    String _s = String((char *)val) + "\r\n";
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)_s.c_str(), 6);
}

void xIX03::println(long num, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)num;
    val[1] = uint8_t(num >> 8);
    val[2] = uint8_t(num >> 16);
    val[3] = uint8_t(num >> 24);
    String _s = String((char *)val) + "\r\n";
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)_s.c_str(), 6);
    if (readLSR())
        writeBlock(SC16IS740_THR, val, 6);
}

void xIX03::println(unsigned long num, int base)
{
    uint8_t val[4];
    val[0] = (uint8_t)num;
    val[1] = uint8_t(num >> 8);
    val[2] = uint8_t(num >> 16);
    val[3] = uint8_t(num >> 24);
    String _s = String((char *)val) + "\r\n";
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)_s.c_str(), 6);
}

void xIX03::println(double num, int digits)
{
    String _s = String(num);
    uint8_t i = sizeof(_s);
    char c[i + digits];
    dtostrf(num, i, digits, c);
    Wire.beginTransmission(i2cAddr);
    Wire.write(0x00);
    for (uint8_t k = 0; k < digits + i; k++)
    {
        Wire.write(c[k]);
    }
    Wire.write('\r');
    Wire.write('\n');
    Wire.endTransmission();
    delay(100);
}

void xIX03::write(uint8_t val)
{
    if (readLSR())
        writeByte(SC16IS740_THR, val);
}

void xIX03::write(const char *str)
{
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)str, strlen(str));
}

void xIX03::write(const uint8_t *buffer, uint8_t size)
{
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)buffer, size);
}

void xIX03::write(const char *buffer, uint8_t size)
{
    if (readLSR())
        writeBlock(SC16IS740_THR, (uint8_t *)buffer, size);
}

void xIX03::flush()
{
    uint8_t reg = readByte(SC16IS740_FCR);
    writeByte(SC16IS740_FCR, reg | 0x06);
}

uint8_t xIX03::available()
{
    return readByte(SC16IS740_RXLVL);
}

uint8_t xIX03::availableForWrite()
{
    return 64 - readByte(SC16IS740_TXLVL);
}

uint8_t xIX03::read()
{
    if (available() == 0)
        return 0;
    return readByte(SC16IS740_RHR);
}

void xIX03::end()
{
}

void xIX03::pinMode(uint8_t pin, uint8_t mode)
{
    uint8_t _reg = readByte(SC16IS740_IODIR);
    if (mode == OUTPUT) {
        _reg |= 1 << pin;
    } else {
        _reg &= ~(1 << pin);
    }
    writeByte(SC16IS740_IODIR, _reg);
}
void xIX03::digitalWrite(uint8_t pin, uint8_t state)
{
    uint8_t _reg = readByte(SC16IS740_IOSTATE);
    //SerialUSB.println(_reg);
    if (state == HIGH) {
        _reg |= 1 << pin;
    } else {
        _reg &= ~(1 << pin);
    }
    writeByte(SC16IS740_IOSTATE, _reg);
}
bool xIX03::digitalRead(uint8_t pin)
{
    uint8_t _reg = readByte(SC16IS740_IOSTATE);
    return (_reg&pin)>>pin;
}
void xIX03::writeByte(uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(i2cAddr);
    Wire.write(reg << 3);
    Wire.write(val);
    Wire.endTransmission();
}

uint8_t xIX03::readByte(uint8_t reg)
{
    Wire.beginTransmission(i2cAddr);
    Wire.write(reg << 3);
    Wire.endTransmission();
    Wire.requestFrom(i2cAddr, (uint8_t)1);
    return Wire.read();
}

void xIX03::writeBlock(uint8_t reg, uint8_t *val, uint8_t len)
{
    Wire.beginTransmission(i2cAddr);
    Wire.write(reg << 3);
    for (uint8_t i = 0; i < len; i++)
    {
        Wire.write(val[i]);
    }
    delay(50);
    Wire.endTransmission();
}