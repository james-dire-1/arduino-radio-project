#include <Wire.h>
#include "RadioHeaders.h"

#define PIN_A A2
#define PIN_B A3
#define ENCODER_BUTTON 12
#define PIN_STICK_X A4
#define PIN_STICK_Y A5
#define STICK_BUTTON 11

RadioHandler handler;
RadioInput input;
RadioDisplay display;
RadioModule module;

void setup() {
  Wire.begin();//kick off the I2C
  Wire.setClock(50000);
  Serial.begin(9600);

  input.Init(PIN_A, PIN_B, ENCODER_BUTTON, PIN_STICK_X, PIN_STICK_Y, STICK_BUTTON);
  display.Init(&handler);
  handler.Init();
}

void loop() {
  static unsigned long lastTime;

  input.Tick();
  display.Tick();

  if (handler.state == Normal) {
    if (input.KnobIsDown()) handler.SwitchBand();

    int joystickPosition = input.JoystickGetRegion();
    if (joystickPosition != 0) handler.TuneToPreset(joystickPosition);

    if (input.KnobIsLongPressed()) {
      handler.state = ChoosePreset;
      display.Clear();
      display.ScrollText(true, "Move  joystick  up,  right,  down,  or  left  to  choose  preset  to  overwrite");
    }

    int knobDirection = input.KnobGetDirection();
    bool updated = handler.UpdateCurrentFrequency(knobDirection);

    if (updated) {
      display.PrintStationData();
      module.TuneTo(handler.band, handler.frequency);
    }
  }

  else if (handler.state == ChoosePreset) {
    int joystickPosition = input.JoystickGetRegion();

    if (joystickPosition != 0) {
      handler.state = PresetChosenConfirmation;
      handler.StorePresetStation(joystickPosition, handler.frequency);

      display.StopScrollText();
      display.Clear();

      display.PrintText(0, true, "Station saved!");

      char toPrint[15];
      sprintf(toPrint, "%s%d", "Preset: ", joystickPosition);
      display.PrintText(1, true, toPrint);

      lastTime = millis();
    }
  }

  else if (handler.state == PresetChosenConfirmation) {
    if ((signed long)millis() - (signed long)lastTime >= display.CONFIRMATION_TIME) {
      handler.state = Normal;
    }
  }
}