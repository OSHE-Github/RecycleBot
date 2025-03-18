#include "config.h"
#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include <FastAccelStepper.h>

int8_t sendFrame(uint16_t id, uint8_t dataLen, uint8_t* data){
  CanFrame txFrame = {0};
  txFrame.extd = 0;
  txFrame.identifier = id & 0xffe;
  txFrame.data_length_code = dataLen;

  for (int i = 0; i < dataLen; i++){
      txFrame.data[i] = data[i];
  }

  return ESP32Can.writeFrame(txFrame);  // timeout defaults to 1 ms
}

uint8_t initCAN(){
  if(ESP32Can.begin(ESP32Can.convertSpeed(500), CAN_TX, CAN_RX, 10, 10)) {
      Serial.println("CAN bus started!");
      return 1;
  } else {
      Serial.println("CAN bus failed!");
      return 0;
  }
}

// Function to convert 4 bytes to a float
float bytesToFloat(byte* data) {
  float value;
  memcpy(&value, data, sizeof(value));
  return value;
}

void setup() {
  Serial.begin(115200);

  // print system info
  Serial.printf("CPU Freq = %d Mhz\r\n", getCpuFrequencyMhz());
  Serial.printf("XTAL Freq = %d Mhz\r\n", getXtalFrequencyMhz());
  Serial.printf("APB Freq = %d Mhz\r\n", getApbFrequency());

  initCAN();
}

void loop() {
  
}