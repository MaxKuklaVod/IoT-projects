#include <SoftwareSerial.h>

SoftwareSerial mySoftwareSerial(10, 11); 

void setup() {
  Serial.begin(9600);
  
  mySoftwareSerial.begin(9600);
}

void loop() {
  if (mySoftwareSerial.available()) {
    char receivedChar = mySoftwareSerial.read();
    mySoftwareSerial.write(receivedChar);
  }
}