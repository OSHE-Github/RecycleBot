#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

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

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}