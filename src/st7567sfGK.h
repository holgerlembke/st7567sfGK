#ifndef st7567sfGK_h
#define st7567sfGK_h
/*
     st7567s for Generation Klick

     st7567s (132X65 Dot Matrix LCD Controller/Drive) i2c lib for the Arduino
   ecosystem.

     by lembke@gmail.com // 2024-01-26 or so.

     Why?

     Ok, I bought these ST7567S from Aliex. And only found one library for
   Arduino that worked without (!) creating a shadow buffer and copying the
   entire buffer for display.

     So while trying this lib my display showed strange errors. Sometimes bit
   flips, sometimes image shifts.

     I first checked the i2c bus pullups and added some Rs to move from 10k to
   ???. Did not help.

     I checked the i2c communication for errors. Yes, there where some.
   Randomly. Strange. Could it be display timing problems? Like "wait x ms after
   ram write"?

     Next step was to look into the libs source and the datasheet. I didn't
   understand what the lib did. So I thought "maybe it is a good idea to rewrite
   it. This way I might find an error and/or understand the lib". I did partial
   rewrites and it didn't help.

     While fishing around I found a note in the arduino forum about two zener
   diodes on the board that seem to be the culprit. It helped, problems are
   gone.

     Meanwhile this lib came to life and was good enough to be published. It is
   quite robust (not immune!) against the errors, so it might be a solution for
   displays that still have their diodes.

     Forum thread:
   https://forum.arduino.cc/t/how-can-i-controll-my-st7567s-lcd-display-128x64-i2c/1146185/7

     Diode-Removal-Instructions:

          3.3~5v [x x x x]
                  c d l a
                  c n c d
                  v g s s

            C █ █ █ █ █ █ █ █ D
            1 █ █ █ █ █ █ █ █ 2
            2   C D R R R R
                1 1 2 4 3 1
                0

     remove D2 + D1. #1 and #6 from the right, small black blobs.

     pendig work:
       -- flip display

     Alt-Shft-F

     History
      + 0.3.11
        - fixed ibrary.properties name     
     ´+ 0.3.10
        - publish

*/
#include <Arduino.h>

class st7567sfGK {
   public:  //---------
    // false+true for colors...
    static constexpr uint8_t colorblack = 1;
    static constexpr uint8_t colorwhite = 0;

    // initialize the display. Wire.begin() needs to be done by your code!
    void begin() { reset(); }
    void begin(uint8_t addr);  // can be 0x3c to 0x3f  p 22/68

    // Resets/Reinits the st7567s
    void reset();

    void constrast(uint8_t value);  // 0x00 .. 0x3f
    void mode(bool on);

    void clear(bool clear);
    void pixel(int x, int y, bool clear);
    void line(int x0, int y0, int x1, int y1, bool clear);
    void circle(int xm, int ym, int r, bool clear, bool solid);

    // Font size is fixed 7x8 pixels, returns next line y-pos
    uint8_t text(uint8_t x, uint8_t y, bool clear, const char* str);
    uint8_t text(uint8_t x, uint8_t y, bool clear, String s) {
        return text(x, y, clear, s.c_str());
    }

   private: //---------
    uint8_t devaddr = 0x3f;  // can be 0x3c to 0x3f  p 22/68

    // read/write-Cache. Works in y-direction, so it is a better idea to walk y
    // first.
    static constexpr uint8_t cacheuninit = 0xff;
    struct c_t {
        uint8_t x = cacheuninit, p, data;
    } cache;

    // Controls that some things are almost a recursion. Almost.
    bool rlock = false;

    void writebasic(uint8_t mode, uint8_t commandordata);
    void writecommand(uint8_t command);
    void writedata(uint8_t data);
    bool readdatabyte(uint8_t x, uint8_t p,
                      uint8_t& data);  // data is referenced.
    void writedatabyte(uint8_t x, uint8_t p, uint8_t data);
    // same as pixel but no range check
    void subpixel(int x, int y, bool clear);

    void writefont(char c, bool clear);
    uint8_t reverse(uint8_t b);
};

#endif