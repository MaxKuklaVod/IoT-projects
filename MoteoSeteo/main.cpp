#define DIR_LEFT 4
#define SPEED_LEFT 5

#define DIR_RIGHT 7
#define SPEED_RIGHT 6

#define FORWARD_LEFT LOW
#define BACKWARD_LEFT HIGH

#define FORWARD_RIGHT HIGH
#define BACKWARD_RIGHT LOW

void move(bool left_dir, int left_speed, bool right_dir, int right_speed) {
  digitalWrite(DIR_LEFT, left_dir);
  digitalWrite(DIR_RIGHT, right_dir);
  analogWrite(SPEED_LEFT, left_speed);
  analogWrite(SPEED_RIGHT, right_speed);
}

void forward(int speed) {
  move(FORWARD_LEFT, speed, FORWARD_RIGHT, speed);
}

void backward(int speed) {
  move(BACKWARD_LEFT, speed, BACKWARD_RIGHT, speed);
}

void turn_left(int steepness) {
  int speed_left = 255 - steepness;
  if (speed_left < 0) speed_left = 0; 
  move(FORWARD_LEFT, speed_left, FORWARD_RIGHT, 255);
}

void turn_right(int steepness) {
  int speed_right = 255 - steepness;
  if (speed_right < 0) speed_right = 0;
  move(FORWARD_LEFT, 255, FORWARD_RIGHT, speed_right);
}

void rotate_left(int speed) {
  move(BACKWARD_LEFT, speed, FORWARD_RIGHT, speed);
}

void rotate_right(int speed) {
  move(FORWARD_LEFT, speed, BACKWARD_RIGHT, speed);
}

void stop() {
  move(FORWARD_LEFT, 0, FORWARD_RIGHT, 0);
}

void setup() {
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);

  forward(200);      
  delay(1000);
  
  rotate_left(150);  
  delay(1000);
  
  backward(200);     
  delay(1000);
  
  stop();            
}

void loop() {
}