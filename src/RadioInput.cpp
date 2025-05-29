#include "RadioHeaders.h"

#define LONG_PRESS_TIME 3000

void RadioInput::Init(int pinA, int pinB, int knobButton, int pinStickX, int pinStickY, int stickButton) {
  encoder = new RotaryEncoder(pinA, pinB, RotaryEncoder::LatchMode::FOUR3);

  pinMode(knobButton, INPUT_PULLUP);
  this->knobButton = knobButton;

  this->pinStickX = pinStickX;
  this->pinStickY = pinStickY;

  pinMode(stickButton, INPUT_PULLUP);
  this->stickButton = stickButton;
}

void RadioInput::Tick() {
  knobLastPressed = knobPressed;
  stickLastPressed = stickPressed;

  encoder->tick();

  if (!stickLastPressed)
    joystickLastDownTime = millis();

  if (!knobLastPressed)
    knobLastDownTime = millis();

  knobPressed = digitalRead(knobButton) == LOW;
  stickPressed = digitalRead(stickButton) == LOW;
}

bool RadioInput::KnobIsDown() {
  return knobPressed && !knobLastPressed;
}

bool RadioInput::KnobIsLongPressed() {
  return millis() - knobLastDownTime >= LONG_PRESS_TIME;
}

int RadioInput::KnobGetDirection() {
  return (int)encoder->getDirection();
}

bool RadioInput::JoystickIsDown() {
  return stickPressed && !stickLastPressed;
}

bool RadioInput::JoystickIsLongPressed() {
  return millis() - joystickLastDownTime >= LONG_PRESS_TIME;
}

int RadioInput::JoystickGetRegion() {
  int rawX = analogRead(pinStickX);
  int rawY = analogRead(pinStickY);

  float x = (rawX / 1023.0f) * 2 - 1;
  float y = (abs(rawY - 1023) / 1023.0f) * 2 - 1;

  float r = sqrt(x * x + y * y);
  float theta = atan2(y, x) * 180 / PI;
  theta += 45;
  if (theta < 0) theta += 360;
  if (theta >= 360) theta -= 360;

  bool inRegion = r >= 1;
  int region = 0;

  if (inRegion && !lastInRegion) {
    if (theta >= 0 && theta < 90) region = 2;
    else if (theta >= 90 && theta < 180) region = 1;
    else if (theta >= 180 && theta < 270) region = 4;
    else if (theta >= 270 && theta < 360) region = 3;
  }

  lastInRegion = inRegion;

  return region;
}