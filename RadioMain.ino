#include <Wire.h>
#include <RotaryEncoder.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#include "RadioHeaders.h"

#define PIN_A A2
#define PIN_B A3
#define PIN_BUTTON 12

RadioHandler handler;
RadioInput input;
RadioDisplay display;
RadioModule module;

void setup() {
  Wire.begin();//kick off the I2C
  Serial.begin(9600);

  input.Init(PIN_A, PIN_B, PIN_BUTTON);
  display.Init(handler);

  display.PrintStationData();
}

void loop() {
  input.KnobTick();

  if (input.KnobIsDown()) {
    handler.SwitchBand();
    display.PrintStationData();
  }

  int knobDirection = input.KnobGetDirection();
  bool updated = handler.UpdateCurrentFrequency(knobDirection);

  if (updated) {
    display.PrintStationData();
    module.TuneTo(handler.band, handler.frequency);
  }
}