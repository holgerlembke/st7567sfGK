
void clockface(int hour, int minute) {
  //--------------------------
  const double sintableuninit = 10000.0;

  static double sintable[60] = { sintableuninit + 1.0 };
  static double costable[60];

  if (sintable[0] > sintableuninit) {
    // Winkel startet bei 3 Uhr mit 0° und läuft gegen den Uhrzeigersinn rum...
    for (int8_t i = 0; i < 60; i++) {
      int8_t z = (i <= 15) ? 15 - i : 60 - i + 15;
      /*war:
      int8_t z = i - 15;
      if (z < 0) {
        z += 60;
      }
      z = 60 - z;*/
      sintable[i] = sin(360.0 / 60.0 * z * PI / 180.0);
      // cos is 1/2 pi zurück
      // costable[i] = cos(360.0 / 60.0 * z * PI / 180.0);
      costable[i < 15 ? 60 - 15 + i : i - 15] = -sintable[i];
    }
  }
  //--------------------------

  // timetable[0]: 17 timetable[1]: 10 timetable[2]: 17 timetable[3]: 11
  hour %= 12;

  static uint8_t timetable[4] = { 0, 0, 120, 100 };
  static uint8_t letztestunde = 100;

  if ((hour == timetable[2]) && (minute == timetable[3])) {
    return;
  }
/*
 Fehldarstellung:
 timetable[0]: 11 timetable[1]: 52 timetable[2]: 11 timetable[3]: 53
 timetable[0]: 11 timetable[1]: 53 timetable[2]: 11 timetable[3]: 54
 timetable[0]: 11 timetable[1]: 54 timetable[2]: 11 timetable[3]: 55

*/

  timetable[0] = timetable[2];
  timetable[1] = timetable[3];
  timetable[2] = hour;
  timetable[3] = minute;

/** /
  for (int i = 0; i < 4; i++) {
    Serial.print(" timetable[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.print(timetable[i]);
  }
  Serial.println();
/**/

  const uint8_t xofs = 32;
  const uint8_t xshift = 64;
  const uint8_t yofs = 32;
  const uint8_t radius = 29;
  const int8_t dshort = -6;
  const int8_t dshort2 = 4;
  const int8_t dlong = 2;

  if (timetable[2] != letztestunde) {
    letztestunde = timetable[2];
    display.clear(st7567sfGK::colorblack);
    for (uint8_t j = 0; j < 2; j++) {
      display.circle(xofs + xshift * j, yofs, radius, st7567sfGK::colorwhite, false);

      for (uint8_t i = 0; i < 12; i++) {
        int8_t k = ( (i % 3 != 0) || (j==1) ) ? dshort2 : 0;
        display.line(xofs + xshift * j + costable[i * 5] * (radius + dshort + k),
                     yofs + sintable[i * 5] * (radius + dshort + k),
                     xofs + xshift * j + costable[i * 5] * (radius + dlong),
                     yofs + sintable[i * 5] * (radius + dlong),
                     st7567sfGK::colorwhite);
      }
    }
  }

  struct points_t {
    uint8_t s1x;
    uint8_t s1y;
    uint8_t s2x;
    uint8_t s2y;
    uint8_t s3x;
    uint8_t s3y;
  } points[4];

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t h1;
    if ((i % 2) == 0) {
      h1 = uint8_t(timetable[i] * 5.0 + timetable[i + 1] / 12.0 + 0.5) % 60;
    } else {
      h1 = timetable[i];
    }
    uint8_t h2 = (h1 + 30 + 5) % 60;
    uint8_t h3 = (h1 + 30 - 5) % 60;

    // Stundenpunkt
    // Ok, hier ist eine Koordinatentransformation nötig, weil 0 Uhr liegt bei 90°
    points[i].s1x = xofs + costable[h1] * (radius + dshort) + (((i % 2) == 0) ? 0 : xshift);
    points[i].s1y = yofs + sintable[h1] * (radius + dshort);
    points[i].s2x = xofs + costable[h2] * (radius + dshort - 5) + (((i % 2) == 0) ? 0 : xshift);
    points[i].s2y = yofs + sintable[h2] * (radius + dshort - 5);
    points[i].s3x = xofs + costable[h3] * (radius + dshort - 5) + (((i % 2) == 0) ? 0 : xshift);
    points[i].s3y = yofs + sintable[h3] * (radius + dshort - 5);
  }

  for (uint8_t i = 0; i < 4; i++) {
    if ((points[i].s1x != points[i + 2].s1x) || (points[i].s1y != points[i + 2].s1y)) {
      display.line(points[i].s1x, points[i].s1y, points[i].s2x, points[i].s2y, i < 2 ? st7567sfGK::colorblack : st7567sfGK::colorwhite);
      display.line(points[i].s1x, points[i].s1y, points[i].s3x, points[i].s3y, i < 2 ? st7567sfGK::colorblack : st7567sfGK::colorwhite);
      display.line(points[i].s2x, points[i].s2y, points[i].s3x, points[i].s3y, i < 2 ? st7567sfGK::colorblack : st7567sfGK::colorwhite);
    }
  }
}

//