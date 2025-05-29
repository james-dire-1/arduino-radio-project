#include "RadioHeaders.h"

void RadioModule::TuneTo(FrequencyBand band, int frequency) {
  if (band == FM) {
    float trueFrequency = (float)frequency / 10.0f;
    setFrequency(trueFrequency);
  }
}

void RadioModule::setFrequency(float frequency) {
  //Code from: http://playground.arduino.cc/Main/TEA5767Radio
  //Datasheet: https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf

  unsigned int frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  // this is from the datasheet

  byte frequencyH = frequencyB >> 8;    //shift over to get the high byte
  byte frequencyL = frequencyB & 0XFF;  //cut off the top to get the LOW
  Wire.beginTransmission(0x60);         //start talking to the radio
  Wire.write(frequencyH);               //1st
  Wire.write(frequencyL);               //2nd
  Wire.write(0xB0);                     //3rd
  Wire.write(0x12);                     //4th
  Wire.write(0x00);                     //5th
  Wire.endTransmission();
  delay(100);
}