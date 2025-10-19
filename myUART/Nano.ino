void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    char receivedChar = Serial.read();
    Serial.write(receivedChar);
  }
}