#include <Wire.h>
#define fire 2
#include <Servo.h>
#include <NewPing.h>
#define trig 5
#define echo 6
Servo servo;
NewPing sonar(trig, echo);
int x = 10;
int y;
unsigned long timer;
unsigned long timer1;
bool flag = 0;


int ref;

void setup(){
  Wire.begin();
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(fire, OUTPUT);
  timer = millis();
  timer1 = millis();
  servo.attach(9);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop(){
  if(Serial.available()){
    ref = Serial.read();
//    Serial.println(ref);
    if(ref == 15){
      digitalWrite(fire, HIGH);
    }else{
      digitalWrite(fire, LOW);
    }if(ref == 0){
      digitalWrite(13, LOW);
    }else{
      digitalWrite(13, HIGH);
    }
    Wire.beginTransmission(9);
    if(ref == 0 || ref == 1 || ref == 14 || ref == 2 || ref == 3 || ref == 4 || ref == 5 || ref == 6 || ref == 7 || ref == 8 || ref == 9 || ref == 10 || ref == 11 || ref == 12 || ref == 13){
      Wire.write(ref);
    }
    Wire.endTransmission();
  }
  if((millis() - timer1) >= 100){
    y = sonar.ping_cm();
    if(y == 0){
      y = 250;
    }
    timer1 = millis();
  }
  if((millis() - timer ) >= 15 && x <= 170 && flag == 0){
    //Serial.write(x);
    servo.write(x);
    x++;
    timer = millis();
  }
  if(x == 171){
    x = 170;
    flag = 1;
  }
  if((millis() - timer ) >= 15 && x >= 10 && flag == 1){
    //Serial.write(x);
    servo.write(x);
    x--;
    timer = millis();
  }
  if(x == 9){
    x = 10;
    flag = 0;
  }
//  Serial.print("x = ");
//  Serial.print(x);
//  Serial.print("\tY = ");
//  Serial.println(y);
  Serial.print("<" + String(x) + "," + String(y) + ">");
}
