#include <Wire.h>
#include <EEPROM.h>
#include <RotaryEncoder.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

enum FrequencyBand {
  FM, AM
};

enum RadioState {
  Normal, ChoosePreset, PresetChosenConfirmation
};

// --------------------

class RadioHandler
{
public:
  RadioState state = Normal;
  FrequencyBand band = FM;
  int frequency = 977;
  int alternativeFrequency = 800;
  int preset = 0;
private:
  bool frequencyChanged = false;
  int savedPresets[4];
  unsigned long stationChangedLastTime = 0;
  bool standbyStationSaved = false;
public:
  void Init();
  void SwitchBand();
  bool UpdateCurrentFrequency(int knobDirection);
  void CheckForPreset();
  void TuneToPreset(int preset);
  void StorePresetStation(int preset, int station);
private:
  int RetrievePresetStation(int preset);
  int GetEEPROMAddress(int preset);
};

// --------------------

class RadioInput
{
private:
  RotaryEncoder* encoder = nullptr;
  int knobButton;
  int pinStickX;
  int pinStickY;
  int stickButton;
  bool knobPressed = false;
  bool knobLastPressed = false;
  bool stickPressed = false;
  bool stickLastPressed = false;
  bool lastInRegion = false;
  unsigned long knobLastDownTime = 0;
  unsigned long joystickLastDownTime = 0;
public:
  void Init(int pinA, int pinB, int knobButton, int pinStickX, int pinStickY, int stickButton);
  void Tick();
  bool KnobIsDown();
  bool KnobIsLongPressed();
  int KnobGetDirection();
  bool JoystickIsDown();
  bool JoystickIsLongPressed();
  int JoystickGetRegion();
};

// --------------------

class RadioDisplay
{
public:
  const long CONFIRMATION_TIME = 3000;
private:
  hd44780_I2Cexp* lcdPtr = nullptr;
  RadioHandler* handler;
  String* textToPrint = nullptr;
  bool repeatScroll;
  int currentCharIndex;
  unsigned long lastTime = 0;
public:
  void Init(RadioHandler* handler);
  void Tick();
  void PrintStationData();
  void PrintText(int row, bool centered, const char* text);
  void ScrollText(bool repeatScroll, const char* text);
  void StopScrollText();
  void Clear();
private:
  void GetFrequencyTypeInfo(FrequencyBand type, String& modulation, String& unit, int& cursorPosition);
  void PrintScrollableText();
};

// --------------------

class RadioModule
{
public:
  void TuneTo(FrequencyBand band, int frequency);
private:
  void setFrequency(float frequency);
};