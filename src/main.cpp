#include <Arduino.h>
#include <SPI.h>
#include "ST7735S.h"

#define FLIP

const uint8_t BL_PIN = 11;
const uint8_t KEY_PINS[5] = { 8, 9, 13, 5, 4 }; // UP, RT, DN, LT, CR

ST7735S<6, 7, 10> lcd;

static void fade(uint8_t from, uint8_t to, uint8_t delta) {
  if (from < to) {
    for (uint16_t f = from; f <= to; f += delta) {
      ledcWrite(0, f);
      delay(50);
    }
  } else {
    for (int16_t f = from; f >= to; f -= delta) {
      ledcWrite(0, f);
      delay(50);
    }
  }
}

static void drawMenuItem(uint8_t index, bool active = false) {
  char str[21];

  sprintf(str, "Item #%u", index);
  lcd.print(0, lcd.charHeight() * index, str, lcd.WHITE, active ? lcd.BLUE : lcd.BLACK);
}

void setup() {
//  Serial.begin(115200);

  for (uint8_t i = 0; i < sizeof(KEY_PINS) / sizeof(KEY_PINS[0]); ++i) {
    pinMode(KEY_PINS[i], INPUT_PULLUP);
  }

  ledcSetup(0, 1000, 8);
  ledcAttachPin(BL_PIN, 0);
//  ledcWrite(0, 127);

  SPI.begin(2, 12, 3, 7);
  lcd.begin();
#ifdef FLIP
  lcd.flip(true);
#endif
  lcd.fill(0, 0, lcd.width(), lcd.height(), lcd.RED);
  fade(0, 128, 4);
  lcd.fill(0, 0, lcd.width(), lcd.height(), lcd.GREEN);
  fade(0, 128, 4);
  lcd.fill(0, 0, lcd.width(), lcd.height(), lcd.BLUE);
  fade(0, 128, 4);
  lcd.fill(0, 0, lcd.width(), lcd.height(), lcd.WHITE);
  fade(0, 128, 4);
  lcd.clear();
/*
  for (char c = ' '; c < 127; ++c) {
    lcd.print(((c - ' ') % (lcd.width() / (lcd.charWidth()))) * (lcd.charWidth()), ((c - ' ') / (lcd.width() / (lcd.charWidth()))) * lcd.charHeight(), c, lcd.WHITE);
  }
  delay(5000);
  lcd.clear();
*/

  lcd.print(0, 0, "        MENU        ", lcd.YELLOW, lcd.GRAY);
  drawMenuItem(1, true);
  drawMenuItem(2);
  drawMenuItem(3);
  drawMenuItem(4);
}

void loop() {
  static uint8_t keys = 0x1F;
  static uint8_t pos = 1;

  for (uint8_t i = 0; i < sizeof(KEY_PINS) / sizeof(KEY_PINS[0]); ++i) {
    if (((keys >> i) & 0x01) != digitalRead(KEY_PINS[i])) {
      if ((keys >> i) & 0x01) {
        drawMenuItem(pos);
        switch (i) {
#ifdef FLIP
          case 2: // DN
#else
          case 0: // UP
#endif
            if (pos > 1)
              --pos;
            else
              pos = 4;
            break;
#ifdef FLIP
          case 3: // LT
#else
          case 1: // RT
#endif
            pos = 4;
            break;
#ifdef FLIP
          case 0: // UP
#else
          case 2: // DN
#endif
            if (pos < 4)
              ++pos;
            else
              pos = 1;
            break;
#ifdef FLIP
          case 1:  // RT
#else
          case 3:  // LT
#endif
            pos = 1;
            break;
          case 4: // CR
            break;
        }
        drawMenuItem(pos, true);
      }
      keys ^= (1 << i);
    }
  }
}
