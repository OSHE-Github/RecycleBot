/*
* Function for finding diameter reading from an infidel sensor
*/

// tuned values for infidel sensor

//const int analogPin = 34;  // 8 channels: GPIO32 - GPIO39 
float dia;
float target = 1.75;

float converter(float x) {
  
  byte numtemps = 9;  // Update the number of rows in the table
  const float table[numtemps][2] = {
    //{ADC reading in, diameter out}
    //CALIBRATION ONLY
    //(unless you want to do the lookup on your host)
    { 993, 0 },
    { 901, 1.38 },
    { 855, 1.50 },
    { 815, 1.61 },
    { 771, 1.70 },
    { 750, 1.75 },
    { 725, 1.81 },
    { 683, 1.93 },
    { 627, 2.09 }
  };  
  byte i;
  float out;

  for (i = 1; i < numtemps; i++) {
    if (table[i][0] < x) {
      float slope = (table[i][1] - table[i - 1][1]) / (table[i][0] - table[i - 1][0]);
      float indiff = (x - table[i - 1][0]);
      float outdiff = slope * indiff;
      out = outdiff + table[i - 1][1];
      return out;
    }
  }
  return 0.0;  // or any default value you prefer
}



float infidelOutput(int channelNumber){
  //Protects from trying to read a digital pin
  /*
  if(!(channelPins[channelNumber] >= A0)){
    return 0;
  }*/

  //Get voltage using Arduino
  return converter(analogRead(channelPins[channelNumber]));
}