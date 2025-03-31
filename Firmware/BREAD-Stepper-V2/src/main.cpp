#include <Arduino.h>
#include <FastAccelStepper.h>
#include <ESP32-TWAI-CAN.hpp>
#include "config.h"

const float targetDiameter = 1.75;

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

CanFrame rxFrame;
bool isHeThere = 0;
uint32_t lastPOST = 0;

bool isDeviceOnCAN() {
  return isHeThere;
}

float bytesToFloat(byte* data) {
  float value;
  memcpy(&value, data, sizeof(value));
  return value;
}

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
      printf("START THIS\n");
      return 1;
  } else {
      Serial.println("CAN bus failed!");
      printf("STOP THIS\n");
      return 1;
  }
}

// jankey circular array queue implementation
float prevDiametersQueue[10] {1.75, 1.75, 1.75, 1.75, 1.75, 1.75, 1.75, 1.75, 1.75, 1.75}; // assume perfect start
const int diameterQueueSize = 10;
int diameterQueueEnd = 0;
float diameterSum = 17.5;
float diameterAverage = 1.75;

// adds reading to queue, checking for validity, updates diameter average of last 10 readings, excluding outliers
void insertDiameterToQueue(float newDiameter) {
  // if most recent reading is an outlier reading (+/- 0.25mm from average ), ignore and return
  if(newDiameter - diameterAverage >= 0.20 || newDiameter - diameterAverage <= 0.20) {
    return;
  }

  // else, update queue with new reading and recalculate average
  float oldDiameter = prevDiametersQueue[diameterQueueEnd];       // grab oldest float value
  prevDiametersQueue[diameterQueueEnd] = newDiameter;             // insert new diameter at end
  diameterQueueEnd = (diameterQueueEnd + 1) % diameterQueueSize;  // update position of end, with rollover at 10
  diameterSum = diameterSum - oldDiameter + newDiameter;          // update sum of last 10 diameters
  diameterAverage = diameterSum / diameterQueueSize;              // update average of last 10 diameters
}

void setup() {  

  Serial.begin(115200);

  pinMode(CAN_TX, OUTPUT);
  pinMode(CAN_RX, OUTPUT);

  uint32_t freq = getCpuFrequencyMhz();
  Serial.printf("CPU Freq = %d Mhz\r\n", freq);
  freq = getXtalFrequencyMhz();
  Serial.printf("XTAL Freq = %d Mhz\r\n", freq);
  freq = getApbFrequency();
  Serial.printf("APB Freq = %d Mhz\r\n", freq);


  initCAN();

  //enable microstepping.
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);

  engine.init();
  stepper = engine.stepperConnectToPin(ST_2);
  if (stepper) {
    stepper->setDirectionPin(DR_2);

    // our motor is 1.8 deg/step
    // so 1 rev = 200 steps
    // and 200 steps/sec = 1 rpm
    // if microstepping on step size divides by 16
    // so 3200 steps/sec = 1 rpm
    stepper->setSpeedInHz(320);  // the parameter is steps/second !!!

    // acceleration is in steps/second per second
    // so 50 steps/sec^2 = 4 seconds to max speed
    stepper->setAcceleration(50);

    // continuous move forwards
    stepper->runForward();
  } else {
    //breaks if error detecting motor.
    return;
  }
}

int stepsPerSec = 20 * 16; // initial speed to pull perfect 1.75mm filament in steps/sec (x16 for microstepping)
const int deltaStepsPerSec = 5; // amount to change speed by to correct
const int speedChangeDelay = 10000; // delay between speed changes in miliseconds
int lastSpeedChange = 0; // last time speed was changed

void loop() {
  
  // set dia to a 0 to default.
  float diameter = 0;
  
  char data[8] = "0000000"; 

  // Read CAN frame for diameter from DAQ
  if(ESP32Can.readFrame(rxFrame, 2000)) {
    printf("Received frame from: %03X. Data is from: ", rxFrame.identifier);
    // print data bytes hex as 
    for(int i = 0; i < rxFrame.data_length_code; i++){
      data[i] = (char) rxFrame.data[i];
      printf("0x%02X, ", rxFrame.data[i]);
    }
    printf("\n");
    isHeThere = true;

    diameter = bytesToFloat(&rxFrame.data[3]);  // Convert 4 bytes to float

    // attempt to insert new reading into queue
    insertDiameterToQueue(diameter);
  } else {
    //Serial.println("He  not there, big sad :(");
    rxFrame = {0};
    isHeThere = false;
  }

  // update speed if it has been long enough
  if(millis() - lastSpeedChange > speedChangeDelay) {
    if(diameterAverage <= targetDiameter - 0.02) { // if filament is <= 1.73mm
      stepsPerSec - deltaStepsPerSec; // slow down for thicker filament
    }
    else if (diameterAverage <= targetDiameter + 0.02) { // if filament is <= 1.77mm
      stepsPerSec + deltaStepsPerSec; // speed up for thicker filament
    }
  }
    
  // change and update target speed
  stepper->setSpeedInHz(stepsPerSec); // the parameter is steps/second !!!
  stepper->runForward();

  printf("Current Speed (steps/sec) : %d\n", stepsPerSec);
}