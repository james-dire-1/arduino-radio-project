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
  Serial.begin(9600);

  input.Init(PIN_A, PIN_B, ENCODER_BUTTON, PIN_STICK_X, PIN_STICK_Y, STICK_BUTTON);
  display.Init(&handler);

  display.PrintStationData();
}

void loop() {
  input.Tick();
  display.Tick();

  if (input.KnobIsDown()) handler.SwitchBand();

  int joystickPosition = input.JoystickGetRegion();
  Serial.print("joystick position ");
  Serial.println(joystickPosition);
  if (joystickPosition != 0) handler.TuneToPreset(joystickPosition);

  if (input.JoystickIsDown()) {
    // TODO
  }

  if (input.JoystickIsLongPressed()) {
    // TODO
  }

  int knobDirection = input.KnobGetDirection();
  bool updated = handler.UpdateCurrentFrequency(knobDirection);

  if (updated) {
    display.PrintStationData();
    module.TuneTo(handler.band, handler.frequency);
  }
}