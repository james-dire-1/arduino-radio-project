#include "RadioHeaders.h"

RadioInput::Init(int pinA, int pinB, int pinButton) {
  encoder = new RotaryEncoder(pinA, pinB, RotaryEncoder::LatchMode::FOUR3);
  pinMode(pinButton, INPUT_PULLUP);
}

void RadioInput::KnobTick() {
  encoder->tick();
}

bool RadioInput::KnobIsDown() {
  static bool lastPressed;

  bool pressed = digitalRead(pinButton) == LOW;

  bool isDown;
  if (pressed && !lastPressed) {
    isDown = true;
  } else {
    isDown = false;
  }

  lastPressed = pressed;

  return isDown;
}

int RadioInput::KnobGetDirection() {
  return (int)encoder->getDirection();
}