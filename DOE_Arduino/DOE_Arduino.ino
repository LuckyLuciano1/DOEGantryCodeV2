#include <Encoder.h>
#include "motors.h"

enum ARDUINO_COMMANDS {//periodic controls sent to arduino
  TURN_ELECTROMAGNETS_OFF,
  TURN_ELECTROMAGNETS_ON,
  TURN_DRILL_OFF,
  TURN_DRILL_ON,
  TURN_FANS_OFF,
  TURN_FANS_ON,
  HOME,
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  CLOSE
};

enum ARDUINO_REPORTS {//periodic reports sent to computer
  NO_ERROR,
  SWITCH_HOME_TRIGGERED,
  SWITCH_MAX_TRIGGERED,
  MOTOR_TARGET_REACHED,//followed by specific motor name
  HOMING_COMPLETE
};

#define ELECTROMAGNETS 5
#define DRILL 6
#define FANS 7

int pololu_CPR = 64;
int pololu_GR = 30;
int pololu_MM_PER_ROT = 5;
int maxon_CPR = 500;
int maxon_GR = 308;
int maxon_MM_PER_ROT = 93;

motor motorZ1("Z1", A9, A8, 21, 20, 39, 41, maxon_GR, maxon_GR, maxon_MM_PER_ROT);
motor motorX1("X1", A7, A6, 19, 17, 31, 33, pololu_GR, pololu_CPR, pololu_MM_PER_ROT);
motor motorY1("Y1", A5, A4, 18, 16, 35, 37, pololu_GR, pololu_CPR, pololu_MM_PER_ROT);
motor motorY2("Y2", A3, A2, 2, 3, 35, 37, pololu_GR, pololu_CPR, pololu_MM_PER_ROT);

void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);
}

void loop() {
  //update motors:
  motorX1.update_motor();
  motorY1.update_motor();
  motorY2.update_motor();
  motorZ1.update_motor();

  //read new info from serial:
  while (Serial.available() > 0) { // if any data available
    char incomingByte = Serial.read(); // read byte
    int char_offset = 65;

    if (incomingByte == TURN_ELECTROMAGNETS_ON + char_offset) {
      digitalWrite(ELECTROMAGNETS, HIGH);
    }
    else if (incomingByte == TURN_ELECTROMAGNETS_OFF + char_offset) {
      digitalWrite(ELECTROMAGNETS, LOW);
    }
    else if (incomingByte == TURN_DRILL_ON + char_offset) {
      digitalWrite(DRILL, HIGH);
    }
    else if (incomingByte == TURN_DRILL_OFF + char_offset) {
      digitalWrite(DRILL, LOW);
    }
    else if (incomingByte == TURN_FANS_ON + char_offset) {
      digitalWrite(FANS, HIGH);
    }
    else if (incomingByte == TURN_FANS_OFF + char_offset) {
      digitalWrite(FANS, LOW);
    }
    else if (incomingByte == MOVE_X + char_offset) {
      int value = read_and_echo_serial();
      motorX1.set_goal(value);
    }
    else if (incomingByte == MOVE_Y + char_offset) {
      int value = read_and_echo_serial();
      motorY1.set_goal(value);
      motorY2.set_goal(value);
    }
    else if (incomingByte == MOVE_Z + char_offset) {
      int value = read_and_echo_serial();
      motorZ1.set_goal(value);
    }
    else if (incomingByte == CLOSE + char_offset) {
      digitalWrite(FANS, LOW);
      digitalWrite(DRILL, LOW);
      digitalWrite(ELECTROMAGNETS, LOW);
      motorX1.stop_motor();
      motorY1.stop_motor();
      motorY2.stop_motor();
      motorZ1.stop_motor();
    }
    else if (incomingByte == HOME + char_offset) {
      homing_sequence();
    }
  }
}

int read_and_echo_serial() {
  char incomingByte = Serial.read();
  int value = 0;
  while (incomingByte != '~') {                     //read the appended numeric value
    if (incomingByte >= 48 && incomingByte <= 57) { //read data if valid
      int i_incomingByte = incomingByte - '0';//char -> int
      value *= 10;
      value += i_incomingByte;
    }
    incomingByte = Serial.read();
  }
  //echo back values:
  char b_value[256];
  itoa(value, b_value, 10);
  Serial.write(b_value);
  return value;
}

void homing_sequence() {
  //-----------------------------------------------------
  //x axis homing:
  motorX1.move_back();
  while (digitalRead(motorX1.switch_home)) { //find 0
  }
  //on finding 0, update position:
  motorX1.stop_motor();
  motorX1.quad->write(0);
  delay(1000);
  motorX1.move_forward();
  while (digitalRead(motorX1.switch_max)) { //find max
    motorX1.cur_pos = motorX1.quad->read();
  }
  //on finding max, update position:
  motorX1.stop_motor();
  motorX1.max_pos = motorX1.cur_pos;
  delay(1000);
  
  motorX1.set_goal(0);
  
  //-----------------------------------------------------
  //y axis homing:
  motorY1.move_back();
  motorY2.move_back();
  while (!digitalRead(motorY1.switch_home)) { //find 0
  }
  //on finding 0, update position:
  motorY1.stop_motor();
  motorY2.stop_motor();
  motorY1.quad->write(0);
  motorY2.quad->write(0);
  delay(1000);
  motorY1.move_forward();
  motorY2.move_forward();
  while (!digitalRead(motorY1.switch_max)) { //find max
    motorY1.cur_pos = motorY1.quad->read();
    motorY2.cur_pos = motorY2.quad->read();
  }
  //on finding max, update position:
  motorY1.stop_motor();
  motorY2.stop_motor();
  motorY1.max_pos = motorY1.cur_pos;
  motorY2.max_pos = motorY2.cur_pos;
  delay(1000);

  motorY1.set_goal(0);
  motorY2.set_goal(0);
  /*
  //-----------------------------------------------------
  //z axis homing:
  motorZ1.move_back();
  while (!digitalRead(motorZ1.switch_home)) { //find 0
  }
  //on finding 0, update position:
  motorZ1.stop_motor();
  motorZ1.quad->write(0);
  delay(1000);
  motorZ1.move_forward();
  while (!digitalRead(motorZ1.switch_max)) { //find max
    motorZ1.cur_pos = motorZ1.quad->read();
  }
  //on finding max, update position:
  motorZ1.stop_motor();
  motorZ1.max_pos = motorZ1.cur_pos;
  delay(1000);
  */
}
