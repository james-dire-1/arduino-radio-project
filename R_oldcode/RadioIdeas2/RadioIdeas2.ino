#include <Wire.h>
#include <RotaryEncoder.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define PIN_ROTARY_A A2
#define PIN_ROTARY_B A3
#define PIN_ROTARY_BUTTON 12

enum FrequencyBand {
  FM, AM
};

RotaryEncoder encoder(PIN_ROTARY_A, PIN_ROTARY_B, RotaryEncoder::LatchMode::FOUR3);
hd44780_I2Cexp lcd;

FrequencyBand band = FM;
int frequency = 977;
int alternativeFrequency = 800;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_ROTARY_BUTTON, INPUT_PULLUP);

  int status = lcd.begin(16, 2);
  if (status) {
    hd44780::fatalError(status);
  }

  PrintStationData();
}

void loop() {
  encoder.tick();
  if (IsDown(PIN_ROTARY_BUTTON)) {
    if (band == FM) band = AM;
    else if (band == AM) band = FM;

    int temporaryFrequency = frequency;
    frequency = alternativeFrequency;
    alternativeFrequency = temporaryFrequency;

    lcd.clear();
    PrintStationData();
  }

  bool updated = UpdateCurrentFrequency();

  if (updated) {
    PrintStationData();
    Tune();
  }
}

bool UpdateCurrentFrequency() {
  int delta = (int) encoder.getDirection();

  if (delta == 0) {
    return false;
  }

  if (band == FM) {

    frequency += delta * 2;
    if (frequency < 875) frequency = 1079;
    else if (frequency > 1079) frequency = 875;

  } else if (band == AM) {

    frequency += delta * 10;
    if (frequency < 530) frequency = 1710;
    else if (frequency > 1710) frequency = 530;

  }

  return true;
}

void PrintStationData() {
  String freq = String(frequency);
  if (band == FM) {
    int length = freq.length();
    freq = freq.substring(0, length-1) + '.' + freq.substring(length-1, length);
  }

  String modulation;
  String unit;
  int cursorPosition;
  GetFrequencyTypeInfo(band, modulation, unit, cursorPosition);

  lcd.setCursor(0, 0);
  lcd.print(modulation);

  lcd.setCursor(6, 0);
  if ((freq.length() == 4 && band == FM) || (freq.length() == 3 && band == AM))
    lcd.print(" ");
  lcd.print(freq);
  
  lcd.setCursor(cursorPosition, 0);
  lcd.print(unit);
}

void Tune() {
  
}

void GetFrequencyTypeInfo(FrequencyBand type, String& modulation, String& unit, int& cursorPosition) {
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

bool IsDown(int pin) {
  static bool lastPressed;

  bool pressed = digitalRead(pin) == LOW;

  bool isDown;
  if (pressed && !lastPressed) {
    isDown = true;
  } else {
    isDown = false;
  }

  lastPressed = pressed;

  return isDown;
}