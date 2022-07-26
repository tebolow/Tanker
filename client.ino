#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#define fire 13
#define magnet 12
#define cannon 14

const char* ssid = "tankerserver";
const char* password = "tanker00";
String path = "http://192.168.4.1";
int ref = 0;
WiFiClient client;
HTTPClient http;
MPU6050 mpu6050(Wire);


void setup() {
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);
  pinMode(fire, INPUT);
  pinMode(magnet, INPUT);
  pinMode(cannon, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    mpu6050.update();
  Serial.print("angleX : ");
  Serial.print(mpu6050.getAngleX());
  Serial.print("\tangleY : ");
  Serial.print(mpu6050.getAngleY());
  Serial.print("\tangleZ : ");
  Serial.println(mpu6050.getAngleZ());
  if(digitalRead(fire) == 0){
    if(digitalRead(cannon) == 0 && digitalRead(magnet) == 0){
      if(mpu6050.getAngleY() < -140){
        ref = 1;
      } else if(mpu6050.getAngleY() > -40 && mpu6050.getAngleX() < 50 && mpu6050.getAngleX() > -50){
        ref = 2;
      }else if(mpu6050.getAngleX() <-50 && mpu6050.getAngleY() > -110 && mpu6050.getAngleY() < -80 && mpu6050.getAngleZ() > 10){
        ref = 3;
      } else if(mpu6050.getAngleX() > 50 && mpu6050.getAngleY() > -140 && mpu6050.getAngleY() < -40 && mpu6050.getAngleZ() < 10){
        ref = 4;
      } else {
        ref = 0;
      }
    }
    if(digitalRead(cannon) == 1){
      if(mpu6050.getAngleY() < -140){
        ref = 6;
      } else if(mpu6050.getAngleY() > -40 && mpu6050.getAngleX() < 50 && mpu6050.getAngleX() > -50){
        ref = 7;
      }else if(mpu6050.getAngleX() <-50 && mpu6050.getAngleY() > -110 && mpu6050.getAngleY() < -80 && mpu6050.getAngleZ() > 10){
        ref = 8;
      } else if(mpu6050.getAngleX() > 50 && mpu6050.getAngleY() > -140 && mpu6050.getAngleY() < -40 && mpu6050.getAngleZ() < 10){
        ref = 9;
      } else {
        ref = 5;
      }
    }
    if(digitalRead(magnet) == 1){
      if(mpu6050.getAngleY() < -140){
        ref = 11;
      } else if(mpu6050.getAngleY() > -40 && mpu6050.getAngleX() < 50 && mpu6050.getAngleX() > -50){
        ref = 12;
      }else if(mpu6050.getAngleX() <-50 && mpu6050.getAngleY() > -110 && mpu6050.getAngleY() < -80 && mpu6050.getAngleZ() > 10){
        ref = 13;
      } else if(mpu6050.getAngleX() > 50 && mpu6050.getAngleY() > -140 && mpu6050.getAngleY() < -40 && mpu6050.getAngleZ() < 10){
        ref = 14;
      } else {
        ref = 10;
      }
    }
  }else {
    ref = 15;
  }
  if(ref == 0){
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  Serial.println(ref);
    String link = path + "/income?ref=" + String(ref);
    //Serial.println(link);
    http.begin(client, link.c_str());
    int responseStatusCode = http.GET();
    //Serial.println(responseStatusCode);
    http.end();
  }
  else{
    Serial.println("WiFi Disconnected");
  }
}
