# st7567sfGK 128x64 i2c LCD driver for Generation Klick

This is a st7567s (132X65 Dot Matrix LCD Controller/Drive) i2c lib for the Arduino ecosystem.

* It works with 128 x 64 liquid crystal displays in black/white,
* it is simple and uses little memory,
* it is driven by I2C,
* supports 3.3 volts for ESP32 and ESP8266,
* works very good with Zener-Diode-modified boards and
* works fair with a NON-Zener-Diode-modified boards.

If you want to use the unmodified board you might expect some random errors. They are not soooo often
as with other libraries, but they are still there.

# Class overview

I designed 3 different classes to meet complexity and code size.

### st7567sfGKBase

Has all the basic stuff including a simple text output function. Smallest footprint.

### st7567sfGK

This is what you might love: all the Base-stuff plus the luxary of the .print()/.println() interface.

### st7567sfGKAdafruit

Designed to use Adafruit style fonts. It is borderline to useless (Naaaa. It is not. There are great small size fonts!), because most of the time the Adafruit fonts have large pixel numbers not useful for a small 128x64 pixel display. Included is a demo and some fonts that show what to expect.

# Function overview
```
    void constrast(uint8_t value); // 0x00 .. 0x3f
    void mode(bool on);
    void rotatedisplay(bool rotate180);

    void clear(bool clear);
    void pixel(int x, int y, bool clear);
    void line(int x0, int y0, int x1, int y1, bool clear);
    void circle(int xm, int ym, int r, bool clear, bool solid);

    // depricated
    uint8_t text(uint8_t x, uint8_t y, const char* str);
    uint8_t text(uint8_t x, uint8_t y, String s);

    // Print like Serial.
    print()...
    println()...
    void setCursor(uint8_t x, uint8_t y);

    // for the Adafruit Font support
    void setFont(const GFXfont* f = NULL);
    void setFontOffset(int8_t x, int8_t y);
```

It does *not* use a buffer. It uses a 1-byte read/write cache.

# Stuff I used

* https://github.com/luetee/ST7567S_128X64_I2C
* https://github.com/mworkfun/ST7567A_128X32DOT_LCD
* https://github.com/adafruit/Adafruit-GFX-Library
* Arduino core for ESP8266 WiFi chip from https://github.com/esp8266/Arduino
* Arduino core for ESP32 WiFi chip from https://github.com/espressif/arduino-esp32
* Infinidash certification NDA toolkit
