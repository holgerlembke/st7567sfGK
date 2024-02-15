#ifndef st7567sfGK_h
#define st7567sfGK_h
/*
 st7567s (128 x 64 monochrome display) for Generation Klick

 st7567s (132X65 Dot Matrix LCD Controller/Drive) i2c lib for the Arduino
 ecosystem.

 by lembke@gmail.com // 2024-01-26 or so.

Screen layout:
    0/0 is bottom left, x goes left to right, y bottom to top

    Text lines flow from top to bottom (y=56 to 0) or bottom to top (y=0 to 56). 
    texts overflows, if end reached.

 Why this was made
 =================

   Ok, I bought these ST7567S from Aliex. And found only one library for
   Arduino that worked without (!) creating a shadow buffer and copying the
   entire buffer for display.

   So while trying this lib my display showed strange errors. Sometimes bit
   flips, sometimes image shifts.

   I first checked the i2c bus pullups and added some Rs to move from 10k to
   4k6 or so. Did not help.

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
       -- move to ..\libraries\Adafruit_GFX_Library\Fonts ??

     Alt-Shft-F

     History
      + 0.4.???
        - text-draw-modus toptobottom/bottomtotop
      + 0.4.1
        - derive from Print-class to have the .print()/.println() interface
      + 0.3.15
        - removed the prints...
      + 0.3.14
        - fixed checkrange()
      + 0.3.13
        - add rotate180
        - rotate resets cache
      + 0.3.12
        - text output now at any pixel position
      + 0.3.11
        - fixed ibrary.properties name
     ´+ 0.3.10
        - publish

*/
#include <Arduino.h>
#include <Print.h>

class st7567sfGK : public Print {
   public:  //---------
    using Print::write;

    // false+true for colors...
    static constexpr uint8_t colorblack = 1;
    static constexpr uint8_t colorwhite = 0;
    enum textflow_t { bottomtotop, toptobottom };

    // initialize the display. Wire.begin() needs to be done by your code!
    void begin() { reset(); }
    void begin(uint8_t addr);  // can be 0x3c to 0x3f  p 22/68

    // Resets/Reinits the st7567s
    void reset();

    void constrast(uint8_t value);  // 0x00 .. 0x3f
    void mode(bool on);
    void rotatedisplay(bool rotate180);

    void clear(bool clear);
    void pixel(int x, int y, bool clear);
    void line(int x0, int y0, int x1, int y1, bool clear);
    void circle(int xm, int ym, int r, bool clear, bool solid);

    // Depricated. Please use the .print()/.println() interface
    // Font size is fixed 7x8 pixels, returns next line y-pos
    uint8_t text(uint8_t x, uint8_t y, bool clear, const char* str);
    uint8_t text(uint8_t x, uint8_t y, bool clear, String s) {
        return text(x, y, clear, s.c_str());
    }

    void setCursor(uint8_t x, uint8_t y) {
        textinfo.x = x;
        textinfo.y = y;
    };
    void textflow(textflow_t flow);

   private:                  //---------
    uint8_t devaddr = 0x3f;  // can be 0x3c to 0x3f  p 22/68

    // 1-Byte-read/write-Cache. Works in y-direction, so it is a better idea to
    // walk y first.
    static constexpr uint8_t cacheuninit = 0xff;
    struct c_t {
        uint8_t x = cacheuninit, p, data;
    } cache;

    // Controls that some things are almost a recursion. Almost.
    bool rlock = false;
    bool rotate180mode = false;

    void writebasic(uint8_t mode, uint8_t commandordata);
    void writecommand(uint8_t command);
    void writedata(uint8_t data);
    bool readdatabyte(uint8_t x, uint8_t p,
                      uint8_t& data);  // data is referenced.
    void writedatabyte(uint8_t x, uint8_t p, uint8_t data);
    // same as pixel() but no range check
    void subpixel(int x, int y, bool clear);

    struct textinfo_t {
        uint8_t x;
        uint8_t y;
        textflow_t tf;
    } textinfo;
    void writechar(uint8_t& x, uint8_t& y, char c, bool clear);
    virtual size_t write(uint8_t);
};


#endif
