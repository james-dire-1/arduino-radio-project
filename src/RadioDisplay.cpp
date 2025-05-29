#include "RadioHeaders.h"

#define SCROLL_SPEED 150
#define INITIAL_SCROLL_DELAY 2000

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
    if ((signed long)millis() - (signed long)lastTime >= SCROLL_SPEED) {
      lastTime = millis();

      PrintScrollableText();
    }
  }
}

void RadioDisplay::PrintStationData() {
  Clear();

  FrequencyBand band = handler->band;
  int frequency = handler->frequency;
  int preset = handler->preset;

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

  if (preset != 0) {
    lcd.setCursor(0, 1);
    lcd.print("Preset ");
    lcd.print(preset);
  }
}

void RadioDisplay::PrintText(int row, bool centered, const char* text) {
  String toPrint = String(text);

  int startPosition = 0;
  if (centered) {
    startPosition = (16 - toPrint.length()) / 2;
  }

  lcdPtr->setCursor(startPosition, row);
  lcdPtr->print(text);
}

void RadioDisplay::ScrollText(bool repeatScroll, const char* text) {
  this->repeatScroll = repeatScroll;
  textToPrint = new String(text);

  currentCharIndex = 0;
  lastTime = millis() + INITIAL_SCROLL_DELAY;

  PrintScrollableText();
}

void RadioDisplay::StopScrollText() {
  delete textToPrint;
  textToPrint = nullptr;
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

void RadioDisplay::PrintScrollableText() {
  lcdPtr->clear();
  int numVisibleChars = min(textToPrint->length() - currentCharIndex, 16);

  if (numVisibleChars <= 0) {
    if (repeatScroll) {
      currentCharIndex = 0;
      lastTime = millis() + INITIAL_SCROLL_DELAY;
      PrintScrollableText();
      return;
    } else {
      delete textToPrint;
      textToPrint = nullptr;
    }
  }

  String visibleText = textToPrint->substring(currentCharIndex, currentCharIndex + numVisibleChars);

  lcdPtr->setCursor(0, 1);
  lcdPtr->print(visibleText);

  currentCharIndex++;
}