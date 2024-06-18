#ifndef st7567sfGK_h
#define st7567sfGK_h
/*
 st7567s (128 x 64 monochrome display) for Generation Klick

 st7567s (132X65 Dot Matrix LCD Controller/Drive) i2c lib for the Arduino
 ecosystem.

 by lembke@gmail.com // 2024-01-26 or so.

Screen layout:
    0/0 is bottom left, x goes left to right, y bottom to top

    Text lines flow from top to bottom or bottom to top.
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

     History
      + 0.4.5
        - removed "virtual ~st7567sfGKAdafruit();" added in 0.4.4
        - removed inline from both virtual functions write(uint8_t value)
          (https://github.com/holgerlembke/st7567sfGK/issues/1)
      + 0.4.4
        - font.cpp replaced reference to <avr/pgmspace.h> with <Arduino.h>
        - added "virtual ~st7567sfGKAdafruit();" to satisfy stm32 arm compiler
      + 0.4.3
        - cache extended to write-cache with dirty, begincache()/endcache(),
          only used for text output
        - st7567sfGKAdafruit: adds Adafruit-Fonts to st7567sfGK class
      + 0.4.2
        - Split into (at least for now) two classes.
          st7567sfGKBase: has all the drawing stuff and a simple text output
          st7567sfGK: adds all the .print()/.println() goodness from Print-class
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
#include <gfxfont.h>

class st7567sfGKBase {
   public:  //---------
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
    void textflow(textflow_t flow);

   private:  //---------
    enum kommentfixerdings { nur_______________ };
    uint8_t devaddr = 0x3f;  // can be 0x3c to 0x3f  p 22/68

    // Controls that some things are almost a recursion. Almost.
    bool rlock = false;
    bool rotate180mode = false;

    void writebasic(uint8_t mode, uint8_t commandordata);
    void writecommand(uint8_t command);
    void writedata(uint8_t data);
    bool readdatabyte(uint8_t x, uint8_t p,
                      uint8_t& data);  // data is referenced.
    void writedatabyte(uint8_t x, uint8_t p, uint8_t data);
    void writedatabyteForced(uint8_t x, uint8_t p, uint8_t data);

   protected:  //---------
    // 1-Byte-read/write-Cache. Works in y-direction, so it is a better idea to
    // walk y first.
    // will it be any more better with begincache/endcache/dirty algo?
    static constexpr uint8_t cacheuninit = 0xff;
    struct c_t {
        uint8_t x = cacheuninit, p, data, level, dirty;
    } cache;
    void begincache();
    void endcache();

    static constexpr uint8_t uninit = 255;
    // weil die Fonts anders funktionieren: Standardfont von unten nach oben,
    // Adafruit von oben nach unten
    uint8_t tfofs = 8;
    // same as pixel() but no range check
    void subpixel(int x, int y, bool clear);
    void writechar(uint8_t& x, uint8_t& y, char c, bool clear);
    struct textinfo_t {
        uint8_t x;
        uint8_t y;
        uint8_t totalheight = uninit;  // die gesamte höhe des fonts
        uint8_t height;  // die höhe bis zur baseline, totalheight-height sind
                         // die pixel unter der baseline
        int8_t xofs = 0;
        int8_t yofs = 0;
        textflow_t tf;
    } textinfo;
};

// multiple inheritance scares the shit out of me.
// https://www.youtube.com/watch?v=xTWrk4L97d8
class st7567sfGK : public st7567sfGKBase, public Print {
   public:  //---------
    using Print::write;
    // sets the bottom left pixel corner for the following print command
    void setCursor(uint8_t x, uint8_t y) {
        textinfo.x = x;
        textinfo.y = y;
    };

   private:  //---------
    virtual size_t write(uint8_t);
};

// Working with Adafruit-Fonts
class st7567sfGKAdafruit : public st7567sfGKBase, public Print {
   public:  //---------
    st7567sfGKAdafruit() { tfofs = 0; }
    using Print::write;
    void setCursor(uint8_t x, uint8_t y) {
        textinfo.x = x;
        textinfo.y = y;
    };
    void setFont(const GFXfont* f = NULL);
    // for "larger" fonts, the spacing can be reduced. Use (-1,-1) or so. Try
    // and error.
    void setFontOffset(int8_t x, int8_t y) {
        textinfo.xofs = x;
        textinfo.yofs = y;
    }

   protected:
   private:  //---------
    GFXfont* gfxFont;
    void writecharAF(uint8_t& x, uint8_t& y, char c, bool clear);
    virtual size_t write(uint8_t);
};
// btw. great font customizer:
// https://tchapi.github.io/Adafruit-GFX-Font-Customiser/

#endif
