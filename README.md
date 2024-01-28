# Arduino st7567s for Generation Klick

This is a st7567s (132X65 Dot Matrix LCD Controller/Drive) i2c lib for the Arduino ecosystem.

It works

* with 128 x 64 liquid cristal displays in black/white,
* simple and with little memory usage,
* with I2C,
* with 3.3 volts ESP32 and ESP8266,
* very good with Zener-Diode-modified boards and
* fair with a NON-Zener-Diode-modified boards.

If you want to use the unmodified board you might expect some random errors. They are not soooo often
as with other libraries, but they are still there.

# Function overview
```
    void constrast(uint8_t value); // 0x00 .. 0x3f
    void mode(bool on);
    void rotatedisplay(bool rotate180);

    void clear(bool clear);
    void pixel(int x, int y, bool clear);
    void line(int x0, int y0, int x1, int y1, bool clear);
    void circle(int xm, int ym, int r, bool clear, bool solid);

    uint8_t text(uint8_t x, uint8_t y, const char* str);
    uint8_t text(uint8_t x, uint8_t y, String s) { return text(x,y,s.c_str()); }
```

It does *not* use a buffer. It uses a 1-byte read/write cache.

# Stuff I used

* https://github.com/luetee/ST7567S_128X64_I2C
* https://github.com/mworkfun/ST7567A_128X32DOT_LCD
* Arduino core for ESP8266 WiFi chip from https://github.com/esp8266/Arduino
* Arduino core for ESP32 WiFi chip from https://github.com/espressif/arduino-esp32
* Infinidash certification NDA toolkit

