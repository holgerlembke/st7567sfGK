#include <Wire.h>
#include <st7567sfGK.h>

// change to fit your needs
const byte PinSDA = 4;
const byte PinSCL = 5;

// Here it is!
st7567sfGK display;

void setup() {
  delay(5000);
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  delay(5000);

  Serial.print(F("\n\nbasic st7567sfGK demo "));
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
}

void simple() {
  static bool seti = false;

  display.clear(!seti);
  // Rahmen
  display.line(0, 0, 127, 0, seti);
  display.line(0, 63, 127, 63, seti);
  display.line(0, 0, 0, 63, seti);
  display.line(127, 0, 127, 63, seti);

  // Kreuz
  display.line(0, 0, 127, 63, seti);
  display.line(0, 63, 127, 0, seti);

  // one circle lower left edge
  display.circle(20, 20, 20, seti, false);

  // two circles lower left edge
  display.circle(127 - 10, 63 - 10, 10, seti, false);
  display.circle(127 - 30, 63 - 10, 10, seti, false);

  // lower right edge, just inside the outer boy
  display.text(127 - 3 * 7 - 1, 1, seti, "ABC");

  // upper left edge, just inside the outer boy
  display.text(1, 62 - 8, seti, "012");


  delay(4000);
  seti = !seti;
}

void fillcircletest() {
  display.clear(false);
  display.circle(40, 40, 18, true, true);
  delay(4000);
}

void lueteeDemo() {  // demo from luetee, names adapted
  display.clear(st7567sfGK::colorblack);
  for (int a = 0; a < 64; a++) {
    display.pixel(a, a, st7567sfGK::colorwhite);  //display one pixel. X=0-31, Y=0-127
    delay(5);
  }
  for (int a = 0; a < 64; a++) {
    display.pixel(a, a, st7567sfGK::colorblack);  //Does not display a pixel. X=0-31, Y=0-127
    delay(5);
  }
  display.clear(st7567sfGK::colorblack);
  for (int a = 0; a < 64; a++) {
    display.pixel(a, a, st7567sfGK::colorwhite);  //Does not display a pixel. X=0-31, Y=0-127
    delay(5);
  }
  delay(5000);

  display.clear(st7567sfGK::colorblack);
  for (int a = 0; a < 128; a++) {
    display.pixel(a, 0, st7567sfGK::colorwhite);   //display one pixel. X=0-31, Y=0-127
    display.pixel(a, 63, st7567sfGK::colorwhite);  //display one pixel. X=0-31, Y=0-127
  }
  for (int a = 0; a < 64; a++) {
    display.pixel(0, a, st7567sfGK::colorwhite);    //display one pixel. X=0-31, Y=0-127
    display.pixel(127, a, st7567sfGK::colorwhite);  //display one pixel. X=0-31, Y=0-127
    display.pixel(4, a, st7567sfGK::colorwhite);
  }
  display.line(5, 5, 125, 60, st7567sfGK::colorwhite);
  display.line(125, 5, 5, 60, st7567sfGK::colorwhite);
  display.line(53, 31, 73, 31, st7567sfGK::colorwhite);
  display.line(63, 20, 63, 40, st7567sfGK::colorwhite);

  display.circle(20, 30, 10, st7567sfGK::colorwhite, false);
  display.circle(20, 30, 15, st7567sfGK::colorwhite, false);

  display.circle(105, 30, 15, st7567sfGK::colorwhite, true);

  delay(5000);

  display.clear(st7567sfGK::colorblack);
  display.line(53, 31, 73, 31, st7567sfGK::colorwhite);
  display.line(63, 20, 63, 40, st7567sfGK::colorwhite);
  display.circle(90, 30, 10, st7567sfGK::colorwhite, false);
  display.circle(30, 30, 10, st7567sfGK::colorwhite, true);
  delay(5000);

  display.clear(st7567sfGK::colorblack);

  /**/
  //display.cursor(7, 0);   //Character display position. y=0-3, x=0-17
  int ypos = 0;
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "KEYES");  //Maximun 18 characters.
  //display.cursor(0, 1);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "ABCDEFGHIJKLMNOPQR");
  //display.cursor(0, 2);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "123456789+-*/<>=$@");
  //display.cursor(0, 3);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "%^&(){}:;'|?,.~\\[]");
  //display.cursor(0, 4);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "ABCDEFGHIJKLMNOPQR");
  //display.cursor(0, 5);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "123456789+-*/<>=$@");
  //display.cursor(0, 6);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "%^&(){}:;'|?,.~\\[]");
  //display.cursor(0, 7);
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "ABCDEFGHIJKLMNOPQR");
  delay(5000);
}

void texttest() {
  display.clear(st7567sfGK::colorblack);

  int ypos = 0;
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "0123456789+-*/<>=$@");
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "ABCDEFGHIJKLMNOPQR");
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "%^&(){}:;'|?,.~\\[]");
  ypos = display.text(0, ypos, st7567sfGK::colorwhite, "abcdefghijklmnopqr");
  delay(5000);
}

void clocktest() {
  for (int i = 45; i < 60; i++) {
    clockface(11, i);
    delay(2000);
  }
}

void stringtest() {
  display.textflow(st7567sfGK::toptobottom);
  display.clear(st7567sfGK::colorblack);
  display.line(0, 0, 50, 50, st7567sfGK::colorwhite);


  display.print("Hallo");
  display.println("xx");
  display.println("0334524");
  display.println("abcdefgh");
}

void loop() {
  // simple();
  // fillcircletest();
  // lueteeDemo();
  // texttest();
  // clocktest();
  stringtest();
  delay(8000);
}

//