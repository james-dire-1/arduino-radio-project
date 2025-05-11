#include "RadioHeaders.h"

void RadioHandler::SwitchBand() {
  if (band == FM) band = AM;
  else if (band == AM) band = FM;

  int temporaryFrequency = frequency;
  frequency = alternativeFrequency;
  alternativeFrequency = temporaryFrequency;
}

bool RadioHandler::UpdateCurrentFrequency(int knobDirection) {
  int delta = knobDirection;

  if (delta == 0) {
    return false;
  }

  if (band == FM) {

    frequency += delta * 2;
    if (frequency < 875) frequency = 1079;
    else if (frequency > 1079) frequency = 875;

  } else if (band == AM) {

    frequency += delta * 10;
    if (frequency < 530) frequency = 1710;
    else if (frequency > 1710) frequency = 530;

  }

  return true;
}