#include <Wire.h>
#include <Servo.h>






const byte npulse = 12; // number of pulses to charge the capacitor before each measurement
const byte pin_pulse = A0; // sends pulses to charge the capacitor (can be a digital pin)
const byte pin_cap  = A1; // measures the capacitor charge
const byte pin_LED = 13; // LED that turns on when metal is detected
const int nmeas = 256; //measurements to take
long int sumsum = 0; //running sum of 64 sums
long int skip = 0; //number of skipped sums
long int diff = 0;      //difference between sum and avgsum
long int flash_period = 0; //period (in ms)
long unsigned int prev_flash = 0; //time stamp of previous flash









Servo servoX;
Servo servoY;

#define mlf 2
#define mlb 4
#define mli 3
#define mrf 7
#define mrb 8
#define mri 5
#define magnet 12

int ref, i = 90, j = 130;
unsigned long time0;

void setup() {
  servoX.attach(9);
  servoY.attach(6);
  servoY.write(j);
  servoX.write(150);
  pinMode(mlf, OUTPUT);
  pinMode(mlb, OUTPUT);
  pinMode(mrf, OUTPUT);
  pinMode(mrb, OUTPUT);
  pinMode(mli, OUTPUT);
  pinMode(mri, OUTPUT);
  pinMode(magnet, OUTPUT);
  pinMode(13, OUTPUT);
  Wire.begin(9);
  Serial.begin(115200);
  Wire.onReceive(receiveEvent);
  time0 = millis();
  pinMode(pin_pulse, OUTPUT);
  digitalWrite(pin_pulse, LOW);
  pinMode(pin_cap, INPUT);
  pinMode(pin_LED, OUTPUT);
  digitalWrite(pin_LED, LOW);
}

void loop() {
  detect();
  if (ref != 10 || ref != 11 || ref != 12 || ref != 13 || ref != 14){
    digitalWrite(magnet, LOW);
  }
  if(ref == 5 || ref == 6 || ref == 7 || ref == 8 || ref == 9 ){
    movement(0,0,0,0,0,0);
  }
  if(ref == 0){
    movement(0,0,0,0,0,0);
//    Serial.println(0);
  }
  else if(ref == 1){
    movement(1,0,1,0,100,100);
//    Serial.println(1);
  }
  else if(ref == 2){
    movement(0,1,0,1,100,100);
//    Serial.println(2);
  }
  else if(ref == 3){
    movement(1,0,0,1,150,150);
//    Serial.println(3);
  }
  else if(ref == 4){
    movement(0,1,1,0,150,150);
//    Serial.println(4);
  }
  else if(ref == 6){
    if((millis() - time0) >= 50 && j > 130){
        servoY.write(j);
        j--;
        time0 = millis();
      }
  }
  else if(ref == 7){
    if((millis() - time0) >= 50 && j < 180){
        servoY.write(j);
        j++;
        time0 = millis();
      }
  }
  else if(ref == 8){
    if((millis() - time0) >= 10 && i > 0){
        servoX.write(i);
        i--;
        time0 = millis();
      }
  }
  else if(ref == 9){
    if((millis() - time0) >= 10 && i < 180){
        servoX.write(i);
        i++;
        time0 = millis();
      }
  }
  else if(ref == 10){
    digitalWrite(magnet, HIGH);
    movement(0,0,0,0,0,0);
//    Serial.println(10);
  }
  else if(ref == 11){
    digitalWrite(magnet, HIGH);
    movement(1,0,1,0,100,100);
  }
  else if(ref == 12){
    digitalWrite(magnet, HIGH);
    movement(0,1,0,1,100,100);
  }
  else if(ref == 13){
    digitalWrite(magnet, HIGH);
    movement(1,0,0,1,150,150);
  }
  else if(ref == 14){
    digitalWrite(magnet, HIGH);
    movement(0,1,1,0,150,150);
  }
}

void receiveEvent(){
while(Wire.available()){
  ref = Wire.read();
//  Serial.println(ref);
}
}

void movement(int a, int b, int c, int d, int e, int f){
digitalWrite(mlf, a);
digitalWrite(mlb, b);
digitalWrite(mrf, c);
digitalWrite(mrb, d);
analogWrite(mli, e);
analogWrite(mri, f);
}

void detect(){
  int minval = 2000;
  int maxval = 0;
 
  //perform measurement
  long unsigned int sum = 0;
  for (int imeas = 0; imeas < nmeas + 2; imeas++) {
    //reset the capacitor
    pinMode(pin_cap, OUTPUT);
    digitalWrite(pin_cap, LOW);
//    delayMicroseconds(20);
    pinMode(pin_cap, INPUT);
    //apply pulses
    for (int ipulse = 0; ipulse < npulse; ipulse++) {
      digitalWrite(pin_pulse, HIGH); //takes 3.5 microseconds
//      delayMicroseconds(3);
      digitalWrite(pin_pulse, LOW); //takes 3.5 microseconds
//      delayMicroseconds(3);
    }
    //read the charge on the capacitor
    int val = analogRead(pin_cap); //takes 13x8=104 microseconds
    minval = min(val, minval);
    maxval = max(val, maxval);
    sum += val;
 
    //determine if LEDs should be on or off
    long unsigned int timestamp = millis();
    byte ledstat = 0;
    if (timestamp < prev_flash +12) {
      if (diff > 0)ledstat = 1;
      if (diff < 0)ledstat = 2;
    }
    if (timestamp > prev_flash + flash_period) {
      if (diff > 0)ledstat = 1;
      if (diff < 0)ledstat = 2;
      prev_flash = timestamp;
    }
    if (flash_period > 1000)ledstat = 0;
 
    //switch the LEDs to this setting
    if (ledstat == 0) {
      digitalWrite(pin_LED, LOW);
    }
    if (ledstat == 1) {
      digitalWrite(pin_LED, LOW);
    }
    if (ledstat == 2) {
      digitalWrite(pin_LED, HIGH);
    }
 
  }
 
  //subtract minimum and maximum value to remove spikes
  sum -= minval; sum -= maxval;
 
  //process
  if (sumsum == 0) sumsum = sum << 6; //set sumsum to expected value
  long int avgsum = (sumsum + 32) >> 6;
  diff = sum - avgsum;
  if (abs(diff)<avgsum >> 10) {   //adjust for small changes
    sumsum = sumsum + sum - avgsum;
    skip = 0;
  } else {
    skip++;
  }
  if (skip > 64) {  // break off in case of prolonged skipping
    sumsum = sum << 6;
    skip = 0;
  }
 
  // one permille change = 2 ticks/s
  if (diff == 0) flash_period = 1000000;
  else flash_period = avgsum / (2 * abs(diff));
}
