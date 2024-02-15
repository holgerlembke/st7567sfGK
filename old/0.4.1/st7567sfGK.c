#include <Arduino.h>
#include <Wire.h>
#include <st7567sfGK.h>

#include <font.cpp>

#define st7567sfGKshowpanics

// Hlpr
static inline bool checkrange(int x, int y) {
    return (x >= 0) && (x < 128) && (y < 64) && (y >= 0);
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::begin(uint8_t addr) {
    devaddr = addr;
    reset();
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::reset() {
    // bit patter at p 33/68
    // sequence at p 42/69

    // Reset
    writecommand(0xE2);
    delay(100);

    writecommand(0xA2);  // select 1/9 Bias
    writecommand(0xA0);  // select SEG Normal Direction
    writecommand(0xC0);  // select COM Normal Direction
    writecommand(0x24);  // select Regulation Ration=5.0
    constrast(0x28);     //
    writecommand(0x2C);  // Booster ON
    writecommand(0x2E);  // Regulator ON
    writecommand(0x2F);  // Follower ON

    mode(true);
    writecommand(0x40);  // start line adresse = 0x00

    textinfo.tf = toptobottom;
    clear(colorblack);
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::constrast(uint8_t value) {
    writecommand(0x81);          // select Contrast EV1 Command
    writecommand(value & 0x3f);  // select Contrast EV2 0x00 - 0x3f
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::mode(bool on) {  // else off
    // 0xAE: off, 0xAF: on, p 34/68
    if (on) {
        writecommand(0xAF);
    } else {
        writecommand(0xAE);
    }
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::rotatedisplay(bool rotate180) {
    rotate180mode = rotate180;
    cache.x = cacheuninit;
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::writebasic(uint8_t mode, uint8_t commandordata) {
    Wire.beginTransmission(devaddr);  // p 22/68
    // Control byte: C0 = 0 (=last command), A0 = 0 (=following is command)
    Wire.write(mode);
    Wire.write(commandordata);
    uint8_t error = Wire.endTransmission();
    if (error != 0) {
#ifdef st7567sfGKshowpanics
        Serial.print("Panic: writecommand error: ");
        Serial.println(error);
#endif
        // This is almost a recursion!
        delay(100);
        if (!rlock) {
            rlock = true;
            writebasic(mode, commandordata);
            rlock = false;
        }
    }
    /*
  0: success.
  1: data too long to fit in transmit buffer.
  2: received NACK on transmit of address.
  3: received NACK on transmit of data.
  4: other error.
  5: timeout
    */
    // delayMicroseconds(1000);
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
/*
      7  6 5 4 3 2 1 0   7 6 5 4 3 2 1 0       p 22/68
     C0 A0 0 0 0 0 0 0   D D D D D D D D

     c0 = 1 -> stream of control+data
*/
void st7567sfGK::writecommand(uint8_t command) {
    writebasic(0x00, command);  // Control byte: C0 = 0 (=last command), A0 = 0
                                // (=following is command)
}

//**************************************************************************************************************************************
//**************************************************************************************************************************************
void st7567sfGK::writedata(uint8_t data) {
    writebasic(0x40, data);  // Control byte: C0 = 0 (=last command), A0 = 1
                             // (=following is data)
}
/*                                 sda scl gnd vcc
   Ram-Layout                       o   o   o   o
   y
   |
   |7   p1
   |6   p1
   |5
   |4
   |3
   |2
   |1
   |0
   |7   p0
   |6   p0
   |5
   |4
   |3
   |2
   |1
   |0
   o-------------------------------------------------------------------------x
    col 0
                                 flex cable
*/

// read a byte from x-axis/page
//**************************************************************************************************************************************
bool st7567sfGK::readdatabyte(uint8_t x, uint8_t p,
                              uint8_t& data)  // data is referenced.
{
    if ((cache.x == x) && (cache.p == p)) {
        data = cache.data;
        return true;
    }

    cache.x = cacheuninit;

    // p 33/68
    writecommand(0xb0 + p);         // Set Page Address
    writecommand(0x10 + (x >> 4));  // Set Column Address
    writecommand(x % 16);           // Set Column Address

    // p 23/68 Dummy-read + Read
    Wire.requestFrom(devaddr, (uint8_t)2);
    Wire.requestFrom(devaddr, (uint8_t)2);
    uint8_t c = 0;
    while (Wire.available()) {
        data = Wire.read();
        c++;
    }
    if (c == 2) {
        cache.x = x;
        cache.p = p;
        cache.data = data;
        return true;
    } else {
#ifdef st7567sfGKshowpanics
        Serial.println("Panic: Read-error (1)");
#endif
        // This is almost a recursion!
        delay(100);
        if (!rlock) {
            rlock = true;
            bool good = readdatabyte(x, p, data);
            rlock = false;
            return good;
        } else {
            return false;
        }
    }
}

//**************************************************************************************************************************************
void st7567sfGK::writedatabyte(uint8_t x, uint8_t p, uint8_t data) {
    if ((cache.x == x) && (cache.p == p) && (cache.data == data)) {
        return;
    }

    writecommand(0xb0 + p);         // Set Page Address
    writecommand(0x10 + (x >> 4));  // Set Column Address
    writecommand(x % 16);           // Set Column Address
    writedata(data);

    cache.x = x;
    cache.p = p;
    cache.data = data;
}

//**************************************************************************************************************************************
void st7567sfGK::clear(bool clear) {
    cache.x = cacheuninit;

    int page = 0;
    int pageAddr = 0xb0;
    byte d = clear ? 0 : 0xff;

    writecommand(0x40);

    for (page = 0; page < 9; page++) {
        writecommand(pageAddr);
        writecommand(0x10);
        writecommand(0x00);
        for (uint8_t column = 0; column < 128; column++) {
            writedata(d);
        }
        pageAddr++;
    }

    // set text x/y start point
    textflow(textinfo.tf);
}

//**************************************************************************************************************************************
void st7567sfGK::textflow(textflow_t flow) {
    textinfo.tf = flow;
    textinfo.x = 0;
    textinfo.y = (textinfo.tf == toptobottom) ? 63 - 8 : 0;
}

//**************************************************************************************************************************************
void st7567sfGK::subpixel(int x, int y, bool clear) {
    // because everything uses the subpixel(), all we do is implement rotation
    // here.
    if (rotate180mode) {
        x = 127 - x;
        y = 63 - y;
    }

    uint8_t p = y >> 3;  // pages with 8 bit
    uint8_t mask = 1 << (y % 8);

    uint8_t data;
    if (readdatabyte(x, p, data)) {
        if (clear) {
            data &= ~mask;
        } else {
            data |= mask;
        }
        writedatabyte(x, p, data);
    }
}

//**************************************************************************************************************************************
void st7567sfGK::pixel(int x, int y, bool clear) {
    if (!checkrange(x, y)) {
        return;
    }
    subpixel(x, y, clear);
}

//**************************************************************************************************************************************
// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void st7567sfGK::line(int x0, int y0, int x1, int y1, bool clear) {
    if ((!checkrange(x0, y0)) || (!checkrange(x1, y1))) {
        return;
    }

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    for (;;) {
        subpixel(x0, y0, clear);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

//**************************************************************************************************************************************
void st7567sfGK::circle(int xm, int ym, int r, bool clear, bool solid) {
    int x = -r;
    int y = 0;
    int err = 2 - 2 * r; /* II. Quadrant */
    do {
        if (solid) {
            line(xm - x, ym - y, xm - x, ym + y, clear);
            line(xm + x, ym - y, xm + x, ym + y, clear);
        } else {
            pixel(xm + y, ym + x, clear);
            pixel(xm - x, ym + y, clear);
            pixel(xm - y, ym - x, clear);
            pixel(xm + x, ym - y, clear);
        }
        r = err;
        if (r <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
        if (r > x || err > y)
            err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);

    if (solid) {
        line(xm, ym + y, xm, ym - y, clear);
    }
}

//**************************************************************************************************************************************
uint8_t st7567sfGK::text(uint8_t x, uint8_t y, bool clear, const char* str) {
    for (int i = 0; i < strlen(str); i++) {
        writechar(x, y, str[i], clear);
    }
    return y + 8;
}

//**************************************************************************************************************************************
// this is slow but allows to put text at any pixel position.
// might move to ..\libraries\Adafruit_GFX_Library\Fonts
void st7567sfGK::writechar(uint8_t& x, uint8_t& y, char c, bool clear) {
    if (c == 0x0d) {  // cr \r
        textinfo.x = 0;
        return;
    } else if (c == 0x0a) {  // lf \n
        if (textinfo.tf == toptobottom) {
            textinfo.y -= 8;
        } else {
            textinfo.y += 8;
        }
        return;
    } else if ((c >= '0') && (c <= '9')) {
        c = c - '0';
    } else if ((c >= 'a') && (c <= 'z')) {
        c = c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'Z')) {
        c = c - 'A' + 36;
    } else {
        switch (c) {
            case '!': c = 62; break;  // ..
            case '"': c = 63; break;
            case '#': c = 64; break;
            case '$': c = 65; break;
            case '%': c = 66; break;
            case '&': c = 67; break;
            case '\'': c = 68; break;
            case '(': c = 69; break;
            case ')': c = 70; break;
            case '*': c = 71; break;
            case '+': c = 72; break;
            case ',': c = 73; break;
            case '-': c = 74; break;
            case '/': c = 75; break;
            case ':': c = 76; break;
            case ';': c = 77; break;
            case '<': c = 78; break;
            case '=': c = 79; break;
            case '>': c = 80; break;
            case '?': c = 81; break;
            case '@': c = 82; break;
            case '{': c = 83; break;
            case '|': c = 84; break;
            case '}': c = 85; break;
            case '~': c = 86; break;
            case ' ': c = 87; break;
            case '.': c = 88; break;
            case '^': c = 89; break;
            case '_': c = 90; break;
            case '`': c = 91; break;
            case '[': c = 92; break;
            case '\\': c = 93; break;
            case ']': c = 94; break;
        }
    }

    uint8_t f[7];
    for (uint8_t i = 0; i < 7; i++) {
        f[i] = pgm_read_word_near(font_7x8[c] + i);
    }

    for (uint8_t i = 0; i < 7; i++) {
        // inner loop uses y to optimize cache
        for (uint8_t j = 0; j < 8; j++) {
            if (f[i] % 2 == 1) {
                subpixel(x + i, y + 8 - j, clear);
            } else {
                subpixel(x + i, y + 8 - j, !clear);
            }
            f[i] >>= 1;
        }
    }
    x += 7;
}

//**************************************************************************************************************************************
inline size_t st7567sfGK::write(uint8_t value) {
    writechar(textinfo.x, textinfo.y, value, colorwhite);
    return 1;  // assume sucess
}

//.
