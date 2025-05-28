#include "RadioHeaders.h"

#define SCROLL_SPEED 150
#define INITIAL_SCROLL_DELAY 2000

#define SPACE_ROW "                "

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
  Serial.println("AB");

  Clear();

  Serial.println("A");

  FrequencyBand band = handler->band;
  int frequency = handler->frequency;

  Serial.println("B");

  String freq = String(frequency);
  if (band == FM) {
    int length = freq.length();
    freq = freq.substring(0, length - 1) + '.' + freq.substring(length - 1, length);
  }

  Serial.println("C");

  String modulation;
  String unit;
  int cursorPosition;
  GetFrequencyTypeInfo(band, modulation, unit, cursorPosition);

  Serial.println("D");

  hd44780_I2Cexp lcd = *lcdPtr;

  Serial.println("E");

  lcd.setCursor(0, 0);
  lcd.print(modulation);

  Serial.println("F");

  lcd.setCursor(6, 0);
  if ((freq.length() == 4 && band == FM) || (freq.length() == 3 && band == AM))
    lcd.print(" ");
  lcd.print(freq);

  Serial.println("G");

  lcd.setCursor(cursorPosition, 0);
  lcd.print(unit);

  Serial.println("H");

  int preset = handler->preset;
  if (preset != 0) {
    lcd.setCursor(0, 1);
    lcd.print("Preset ");
    lcd.print(preset);
  }

  Serial.println("I");

  Serial.println("Reached the end of PrintStationData");
}

void RadioDisplay::PrintText(bool centered, const char* text) {
  String toPrint = String(text);

  int startPosition = 0;
  if (centered) {
    startPosition = (16 - toPrint.length()) / 2;
  }

  lcdPtr->setCursor(startPosition, 1);
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
  Serial.println("ABC");

  // lcdPtr->clear();
  Serial.println("001");
  lcdPtr->setCursor(0, 0);
  Serial.println("002");
  lcdPtr->print(SPACE_ROW);
  Serial.println("003");
  lcdPtr->setCursor(0, 0);
  Serial.println("003");
  lcdPtr->print(SPACE_ROW);

  Serial.println("ABCD");
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

  currentCharIndex++;
}