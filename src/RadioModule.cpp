#include "RadioHeaders.h"

void RadioModule::TuneTo(FrequencyBand band, int frequency) {
  Serial.println("R000");

  if (band == FM) {
    float trueFrequency = (float)frequency / 10.0f;
    setFrequency(trueFrequency);
  }

  Serial.println("R001");

  Serial.println("Reached the end of TuneTo");
}

void RadioModule::setFrequency(float frequency) {
  //Code from: http://playground.arduino.cc/Main/TEA5767Radio
  //Datasheet: https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf

  unsigned int frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  // this is from the datasheet

  byte frequencyH = frequencyB >> 8;    //shift over to get the high byte
  byte frequencyL = frequencyB & 0XFF;  //cut off the top to get the LOW
  Serial.println("R002");
  Wire.beginTransmission(0x60);         //start talking to the radio
  Serial.println("R003");
  Wire.write(frequencyH);               //1st
  Serial.println("R004");
  Wire.write(frequencyL);               //2nd
  Serial.println("R005");
  Wire.write(0xB0);                     //3rd
  Serial.println("R006");
  Wire.write(0x12);                     //4th
  Serial.println("R007");
  Wire.write(0x00);                     //5th
  Serial.println("R008");
  int result = Wire.endTransmission();
  Serial.println("result ");
  Serial.println(result);
  Serial.println("R009");
  delay(100);
}