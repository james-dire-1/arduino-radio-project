#include "RadioHeaders.h"

void RadioInput::Init(int pinA, int pinB, int encoderButton, int pinStickX, int pinStickY, int stickButton) {
  encoder = new RotaryEncoder(pinA, pinB, RotaryEncoder::LatchMode::FOUR3);

  pinMode(encoderButton, INPUT_PULLUP);
  this->encoderButton = encoderButton;

  this->pinStickX = pinStickX;
  this->pinStickY = pinStickY;

  pinMode(stickButton, INPUT_PULLUP);
  this->stickButton = stickButton;
}

void RadioInput::Tick() {
  encoder->tick();
}

bool RadioInput::KnobIsDown() {
  return IsDown(encoderButton, encoderLastPressed);
}

int RadioInput::KnobGetDirection() {
  return (int)encoder->getDirection();
}

int RadioInput::JoystickGetRegion() {
  static bool lastInRegion;

  int realX = analogRead(A1);
  int realY = analogRead(A2);

  float x = (realX / 1023.0f) * 2 - 1;
  float y = (abs(realY - 1023) / 1023.0f) * 2 - 1;

  float r = sqrt(x * x + y * y);
  float theta = atan2(y, x) * 180 / PI;
  theta += 22.5;
  if (theta < 0) theta += 360;
  if (theta >= 360) theta -= 360;

  bool inRegion = r >= 1;
  int region = 0;

  if (inRegion && !lastInRegion) {
    lastInRegion = true;

    if (theta >= 20 && theta < 25) region = 3;
    else if (theta >= 25 && theta < 110) region = 2;
    else if (theta >= 110 && theta < 115) region = 1;
    else if (theta >= 115 && theta < 200) region = 8;
    else if (theta >= 200 && theta < 205) region = 7;
    else if (theta >= 205 && theta < 290) region = 6;
    else if (theta >= 290 && theta < 295) region = 5;
    else if ((theta >= 295 && theta <= 359) || (theta >= 0 && theta < 20)) region = 4;
  }

  lastInRegion = inRegion;

  return region;
}

bool RadioInput::JoystickIsDown() {
  return IsDown(stickButton, stickLastPressed);
}

bool RadioInput::JoystickIsLongPressed() {
  // TODO
  return false;
}

bool RadioInput::IsDown(int pinButton, bool& lastPressed) {
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