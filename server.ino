#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
int integerFromPC = 0;
int integerFromPC2 = 0;

const char* ssid = "tankerserver";
const char* password = "tanker00";
int ref;
bool fire, magnet;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP Address: ");
  Serial.println(IP);
  server.on("/", handleOnConnect);
  server.on("/income", handleIncome);
  server.on("/getData", handleGetData);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.print("HTTP server started ");
}

void loop() {
  server.handleClient();
}

void handleIncome(){ 
  ref = server.arg("ref").toInt();
  server.send(200, "text/plain", "received");
  Serial.write(ref);
}

void handleGetData(){
  recvWithStartEndMarkers();
  showNewData();
  server.send(200, "text/plain", receivedChars);
}

void handleOnConnect(){
  server.send(200, "text/plain", "Connected");
}

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
     while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.println(receivedChars);
        newData = false;
    }
}
