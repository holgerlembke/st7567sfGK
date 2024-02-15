#include <Wire.h>
#include <st7567sfGK.h>

#include <adafruitfonts/FreeMono9pt7b.h>
#include <adafruitfonts/FreeSans9pt7b.h>
#include <adafruitfonts/FreeSerif9pt7b.h>
// wirklich sehr kleine fonts
#include <adafruitfonts/Tiny3x3a2pt7b.h>
#include <adafruitfonts/Org_01.h>
#include <adafruitfonts/Picopixel.h>

// change to fit your needs
const byte PinSDA = 4;
const byte PinSCL = 5;

// Here it is!
st7567sfGKAdafruit display;

void setup() {
  delay(5000);
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  delay(5000);

  Serial.print(F("\n\nst7567sfGK demo for Adafruit Fonts "));
  Serial.print(__DATE__);
  Serial.print(F(" "));
  Serial.print(__TIME__);
  Serial.println();

  // st7567sfGK sometimes will hang if µC is resetet at the "right" moment
  I2C_ClearBus(PinSDA, PinSCL);

  // Adapt this to your needs. st7567s can Fast Mode/400 kHz i2c Bus speed
  Wire.begin(PinSDA, PinSCL, 400000);
  // Wire.begin();
  // Wire.begin(400000);

  display.begin();
  display.rotatedisplay(true);

  display.setFont(&Picopixel);
  // tft.setFont(); for default font
}

void stringtest() {
  display.textflow(st7567sfGK::toptobottom);
  display.clear(st7567sfGK::colorblack);
  display.line(0, 0, 63, 63, st7567sfGK::colorwhite);
  display.line(63, 63, 126, 0, st7567sfGK::colorwhite);


  display.print("Hallo/[");
  /**/
  display.println("xyjIO");
  display.println("0334524");
  display.println("abcdefgh");
  /**/
  display.println("jhr");
}

void fontrotatorspeedtest() {
  static byte f = 0;
  const GFXfont* fonts[6] = { &FreeMono9pt7b, &FreeSans9pt7b, &FreeSerif9pt7b, &Tiny3x3a2pt7b, &Org_01, &Picopixel };

  display.setFont(fonts[f]);
  f = f == (sizeof(fonts) / sizeof(fonts[90]) - 1) ? 0 : f + 1;

  display.textflow(st7567sfGK::toptobottom);
  display.clear(st7567sfGK::colorblack);
  display.line(0, 0, 63, 63, st7567sfGK::colorwhite);
  display.line(63, 63, 126, 0, st7567sfGK::colorwhite);

  uint32_t start = millis();
  display.println("Hallo~");
  display.println("0123456789");
  display.println("abcdefghijk");
  uint32_t ende = millis();

  Serial.print("RT: ");
  Serial.print(ende - start);
  Serial.println(" ms");
}

void fonttightener() { // shows how to reduces thze font footprint by -1/-1 pixel 
  display.setFont(&FreeMono9pt7b);

  display.textflow(st7567sfGK::toptobottom);
  display.clear(st7567sfGK::colorblack);

  display.line(0, 0, 63, 63, st7567sfGK::colorwhite);
  display.line(63, 63, 126, 0, st7567sfGK::colorwhite);
  display.println("Hallo~");
  display.println("0123456789");
  display.println("abcdefghijk");

  delay(8000);
  display.textflow(st7567sfGK::toptobottom);
  display.clear(st7567sfGK::colorblack);
  display.setFontOffset(-1, -1);  // <------------------

  display.line(0, 0, 63, 63, st7567sfGK::colorwhite);
  display.line(63, 63, 126, 0, st7567sfGK::colorwhite);
  display.println("Hallo~");
  display.println("0123456789");
  display.println("abcdefghijk");
}

void loop() {
  fonttightener();
  delay(8000);
}

//