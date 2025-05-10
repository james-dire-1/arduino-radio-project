#include <Wire.h>
#include <RotaryEncoder.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

enum FrequencyBand {
  FM, AM
};

// --------------------

class RadioHandler
{
public:
  FrequencyBand band = FM;
  int frequency = 977;
  int alternativeFrequency = 800;
public:
  void SwitchBand();
  bool UpdateCurrentFrequency(int knobDirection);
};

// --------------------

class RadioInput
{
private:
  RotaryEncoder* encoder = nullptr;
  int pinButton;
public:
  Init(int pinA, int pinB, int pinButton);
  void KnobTick();
  bool KnobIsDown();
  int KnobGetDirection();
};

// --------------------

class RadioDisplay
{
private:
  hd44780_I2Cexp* lcdPtr;
  RadioHandler handler;
public:
  void Init(RadioHandler handler);
  void PrintStationData();
  void Clear();
private:
  void GetFrequencyTypeInfo(FrequencyBand type, String& modulation, String& unit, int& cursorPosition);
};

// --------------------

class RadioModule
{
public:
  void TuneTo(FrequencyBand band, int frequency);
private:
  void setFrequency(float frequency);
};