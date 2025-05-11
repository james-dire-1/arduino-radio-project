#include "RadioHeaders.h"

#define FM_EEPROM_START 10
#define AM_EEPROM_START 20

void RadioHandler::SwitchBand() {
  if (band == FM) band = AM;
  else if (band == AM) band = FM;

  int temporaryFrequency = frequency;
  frequency = alternativeFrequency;
  alternativeFrequency = temporaryFrequency;

  frequencyChanged = true;
}

bool RadioHandler::UpdateCurrentFrequency(int knobDirection) {
  if (frequencyChanged) {
    frequencyChanged = false;
    return true;
  }

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

void RadioHandler::TuneToPreset(int preset) {
  frequencyChanged = true;
  this->preset = preset;

  int address = GetEEPROMAddress(preset);
  frequency = (int)EEPROM.read(address);
}

void RadioHandler::StorePresetStation(int preset, int station) {
  int address = GetEEPROMAddress(preset);
  EEPROM.update(address, (uint8_t)station);
}

int RadioHandler::GetEEPROMAddress(int preset) {
  int startAddress = (band == FM) ? FM_EEPROM_START : AM_EEPROM_START;
  return startAddress + preset;
}