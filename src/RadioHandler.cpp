#include "RadioHeaders.h"

#define INIT_ADDRESS 30
#define FM_EEPROM_START 10
#define AM_EEPROM_START 20
#define STANDBY_STATION_ADDRESS 40

#define FM_LOWEST 875
#define FM_HIGHEST 1079
#define AM_LOWEST 530
#define AM_HIGHEST 1710

#define TIME_UNTIL_STANDBY_SAVE 10000

void RadioHandler::Init() {
  if (EEPROM.read(INIT_ADDRESS) == 0xff) {
    EEPROM.update(INIT_ADDRESS, 10);

    for (int j = 1; j <= 2; j++) {
      int startAddress = (j == 1) ? FM_EEPROM_START : AM_EEPROM_START;
      int defaultStation = (j == 1) ? FM_LOWEST : AM_LOWEST;

      for (int i = 1; i <= 4; i++) {
        int address = startAddress + i * sizeof(unsigned short);
        unsigned short valueStored;
        EEPROM.get(address, valueStored);

        if (valueStored == 0xffff) StorePresetStation(i, defaultStation);
      }
    }

    EEPROM.put(STANDBY_STATION_ADDRESS, (unsigned short)FM_LOWEST);
  }

  for (int i = 0; i < 4; i++) {
    savedPresets[i] = RetrievePresetStation(i + 1);
  }

  unsigned short standbyStation;
  EEPROM.get(STANDBY_STATION_ADDRESS, standbyStation);
  frequency = standbyStation;

  frequencyChanged = true;
}

void RadioHandler::SwitchBand() {
  frequencyChanged = true;

  if (band == FM) band = AM;
  else if (band == AM) band = FM;

  int temporaryFrequency = frequency;
  frequency = alternativeFrequency;
  alternativeFrequency = temporaryFrequency;
}

bool RadioHandler::UpdateCurrentFrequency(int knobDirection) {
  if (band == FM) {
    if (!standbyStationSaved && 
        (signed long)millis() - (signed long)stationChangedLastTime > TIME_UNTIL_STANDBY_SAVE) {

      standbyStationSaved = true;
      EEPROM.put(STANDBY_STATION_ADDRESS, (unsigned short)frequency);
    }
  }

  if (frequencyChanged) {
    frequencyChanged = false;
    stationChangedLastTime = millis();
    standbyStationSaved = false;
    return true;
  }

  int delta = knobDirection;

  if (delta == 0)
    return false;

  preset = 0;
  stationChangedLastTime = millis();
  standbyStationSaved = false;

  if (band == FM) {

    frequency += delta * 2;
    if (frequency < FM_LOWEST) frequency = FM_HIGHEST;
    else if (frequency > FM_HIGHEST) frequency = FM_LOWEST;

  } else if (band == AM) {

    frequency += delta * 10;
    if (frequency < AM_LOWEST) frequency = AM_HIGHEST;
    else if (frequency > AM_HIGHEST) frequency = AM_LOWEST;

  }

  return true;
}

void RadioHandler::CheckForPreset() {
  for (int i = 0; i < 4; i++) {
    if (savedPresets[i] == frequency) {
      preset = i + 1;
      break;
    }
  }
}

void RadioHandler::TuneToPreset(int preset) {
  frequencyChanged = true;
  this->preset = preset;

  frequency = RetrievePresetStation(preset);
}

int RadioHandler::RetrievePresetStation(int preset) {
  int address = GetEEPROMAddress(preset);
  unsigned short station;
  EEPROM.get(address, station);

  return (int)station;
}

void RadioHandler::StorePresetStation(int preset, int station) {
  int address = GetEEPROMAddress(preset);
  EEPROM.put(address, (unsigned short)station);

  savedPresets[preset - 1] = station;
}

int RadioHandler::GetEEPROMAddress(int preset) {
  int startAddress = (band == FM) ? FM_EEPROM_START : AM_EEPROM_START;
  return startAddress + preset * sizeof(unsigned short);
}