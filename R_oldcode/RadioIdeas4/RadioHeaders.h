#include <Wire.h>
#include <EEPROM.h>
#include <RotaryEncoder.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

enum FrequencyBand {
  FM, AM
};

enum RadioState {
  Normal, Settings
};

// --------------------

class RadioHandler
{
public:
  FrequencyBand band = FM;
  RadioState state = Normal; // TODO Change visibility if necessary
  int frequency = 977;
  int alternativeFrequency = 800;
private:
  bool frequencyChanged = false;
  int preset = 0;
public:
  void SwitchBand();
  bool UpdateCurrentFrequency(int knobDirection);
  void TuneToPreset(int preset);
  void StorePresetStation(int preset, int station);
  int RetreivePresetStation(int preset);
private:
  int GetEEPROMAddress(int preset);
};

// --------------------

class RadioInput
{
private:
  RotaryEncoder* encoder = nullptr;
  int encoderButton;
  int pinStickX;
  int pinStickY;
  int stickButton;
  bool encoderLastPressed = false;
  bool stickLastPressed = false;
public:
  Init(int pinA, int pinB, int encoderButton, int pinStickX, int pinStickY, int stickButton);
  void Tick();
  bool KnobIsDown();
  int KnobGetDirection();
  int JoystickGetRegion();
  bool JoystickIsDown();
  bool JoystickIsLongPressed();
private:
  bool IsDown(int pinButton, bool& lastPressed);
};

// --------------------

class RadioDisplay
{
private:
  hd44780_I2Cexp* lcdPtr = nullptr;
  RadioHandler* handler;
  String* textToPrint = nullptr;
  bool repeatScroll;
  int currentCharIndex;
  unsigned long lastTime;
public:
  void Init(RadioHandler* handler);
  void Tick();
  void PrintStationData();
  void ScrollText(bool repeatScroll, const char* text);
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