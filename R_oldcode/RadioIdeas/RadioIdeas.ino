#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define PIN_CLK 3
#define PIN_DT 4

enum FrequencyType {
  FM, AM
};

hd44780_I2Cexp lcd;

FrequencyType type = AM;
int frequency = 977;
// int amFrequency = 800;
int lastFrequency;

volatile int lastClkState = LOW;
volatile int count;
int lastCount;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_DT, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_CLK), pollRotaryEncoder, CHANGE);

  int status = lcd.begin(16, 2);
  if (status) {
    hd44780::fatalError(status);
  }
}

void loop() {
  delay(100);

  UpdateCurrentFrequency();

  if (frequency != lastFrequency) {
    lastFrequency = frequency;

    PrintStationData();
  }

  Serial.print("Count: ");
  Serial.println(count);
}

void UpdateCurrentFrequency() {
  int currentCount = count;

  if (currentCount != lastCount) {
    int delta = currentCount - lastCount;

    if (type == FM) {
      frequency += delta * 2;
    } else if (type == AM) {
      frequency += delta * 10;
    }

    lastCount = currentCount;
  }
}

void PrintStationData() {
  String freq = String(frequency);
  if (type == FM) {
    int length = freq.length();
    freq = freq.substring(0, length-1) + '.' + freq.substring(length-1, length);
  }

  String modulation;
  String unit;
  int cursorPosition;
  GetFrequencyTypeInfo(type, modulation, unit, cursorPosition);

  lcd.setCursor(0, 0);
  lcd.print(modulation);

  lcd.setCursor(5, 0);
  if ((freq.length() == 4 && type == FM) || (freq.length() == 3 && type == AM))
    lcd.print(" ");
  lcd.print(freq);
  
  lcd.setCursor(cursorPosition, 0);
  lcd.print(unit);
}

void GetFrequencyTypeInfo(FrequencyType type, String& modulation, String& unit, int& cursorPosition) {
  if (type == FM) {
    modulation = "FM";
    unit = "MHz";
    cursorPosition = 11;
  } else if (type == AM) {
    modulation = "AM";
    unit = "kHz";
    cursorPosition = 10;
  }
}

void pollRotaryEncoder() {
  int currentClkState = digitalRead(PIN_CLK);

  if (lastClkState == LOW && currentClkState == HIGH) {
    if (digitalRead(PIN_DT) == HIGH) {
      count--;
    } else {
      count++;
    }
  }
}