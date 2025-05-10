#include "RadioHeaders.h"

#define SCROLL_SPEED 1000
#define INITIAL_SCROLL_DELAY 3000

void RadioDisplay::Init(RadioHandler* handler) {
  this->handler = handler;
  lcdPtr = new hd44780_I2Cexp();

  int status = lcdPtr->begin(16, 2);
  if (status) {
    hd44780::fatalError(status);
  }
}

void RadioDisplay::Tick() {
  if (textToPrint != nullptr) {
    if (millis() - lastTime >= SCROLL_SPEED) {
      lastTime = millis();
      currentCharIndex++;
      
      int numVisibleChars = min(textToPrint->length() - currentCharIndex, 16);

      if (numVisibleChars <= 0) {
        if (repeatScroll) {
          currentCharIndex = 0;
          return;
        } else {
          delete textToPrint;
          textToPrint = nullptr;
        }
      }

      String visibleText = textToPrint->substring(currentCharIndex, currentCharIndex + numVisibleChars);

      lcdPtr->setCursor(0, 1);
      lcdPtr->print(visibleText);
    }
  }
}

void RadioDisplay::PrintStationData() {
  Clear();

  FrequencyBand band = handler->band;
  int frequency = handler->frequency;

  String freq = String(frequency);
  if (band == FM) {
    int length = freq.length();
    freq = freq.substring(0, length - 1) + '.' + freq.substring(length - 1, length);
  }

  String modulation;
  String unit;
  int cursorPosition;
  GetFrequencyTypeInfo(band, modulation, unit, cursorPosition);

  hd44780_I2Cexp lcd = *lcdPtr;

  lcd.setCursor(0, 0);
  lcd.print(modulation);

  lcd.setCursor(6, 0);
  if ((freq.length() == 4 && band == FM) || (freq.length() == 3 && band == AM))
    lcd.print(" ");
  lcd.print(freq);

  lcd.setCursor(cursorPosition, 0);
  lcd.print(unit);

  // TODO Add stuff for handler.preset
}

void RadioDisplay::ScrollText(bool repeatScroll, const char* text) {
  this->repeatScroll = repeatScroll;
  textToPrint = new String(text);
  currentCharIndex = 0;
  lastTime = millis() + INITIAL_SCROLL_DELAY;
}

void RadioDisplay::Clear() {
  lcdPtr->clear();
}

void RadioDisplay::GetFrequencyTypeInfo(FrequencyBand type, String& modulation, String& unit, int& cursorPosition) {
  if (type == FM) {
    modulation = "FM";
    unit = "MHz";
    cursorPosition = 12;
  } else if (type == AM) {
    modulation = "AM";
    unit = "kHz";
    cursorPosition = 11;
  }
}