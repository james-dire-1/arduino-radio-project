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

// Testing another commit over here. Let's see if it commits to the main branch 

void setup() {
  Wire.begin();//kick off the I2C
  Wire.setClock(50000);
  Serial.begin(9600);

  input.Init(PIN_A, PIN_B, ENCODER_BUTTON, PIN_STICK_X, PIN_STICK_Y, STICK_BUTTON);
  display.Init(&handler);
  handler.Init();

  display.PrintStationData();
}

unsigned long lastTimeDebugging;
int count;

void loop() {
  static unsigned long lastTime;

  input.Tick();
  display.Tick();  

  // Serial.println("looping");

  if (handler.state == Normal) {
    if (input.KnobIsDown()) handler.SwitchBand();

    // int joystickPosition = input.JoystickGetRegion();
    // if (joystickPosition != 0) {
      // Serial.print("joystick position ");
      // Serial.println(joystickPosition);
      // handler.TuneToPreset(joystickPosition);
    // }

    // if (input.JoystickIsDown()) {
      // TODO
    // }

    /* if (input.JoystickIsLongPressed()) {
      handler.state = ChoosePreset;
      display.Clear();
      display.ScrollText(true, "Move  joystick  up,  right,  down,  or  left  to  choose  preset  to  overwrite");
    } */

    // int knobDirection = input.KnobGetDirection();
    // bool updated = handler.UpdateCurrentFrequency(knobDirection);

    int fakeKnobDirection = 0;
    if ((signed long)millis() - (signed long)lastTimeDebugging >= 1000) {
      lastTimeDebugging = millis();

      Serial.print("We are changing station ");
      Serial.println(count);
      fakeKnobDirection = 1;

      count++;
    }
    bool updated = handler.UpdateCurrentFrequency(fakeKnobDirection);

    if (updated) {
      // Serial.println("updated");
      // display.PrintStationData();
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
      display.PrintText(true, "Preset saved!");

      lastTime = millis();
    }
  }

  else if (handler.state == PresetChosenConfirmation) {
    if ((signed long)millis() - (signed long)lastTime >= display.CONFIRMATION_TIME) {
      handler.state = Normal;
    }
  }
}