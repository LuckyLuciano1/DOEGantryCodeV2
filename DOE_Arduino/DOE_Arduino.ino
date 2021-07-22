#include "motors.h"

enum ARDUINO_COMMANDS {//periodic controls sent to arduino
  TURN_ELECTROMAGNETS_OFF,
  TURN_ELECTROMAGNETS_ON,
  TURN_DRILL_OFF,
  TURN_DRILL_ON,
  TURN_FANS_OFF,
  TURN_FANS_ON,
  SET_X_PERCENT,
  SET_Y_PERCENT,
  SET_Z_PERCENT,
  HOME,
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  CLOSE
};
//note pin 39 on Arduino ADK appears to be faulty

enum ARDUINO_REPORTS {//periodic reports sent to computer
  NO_ERROR,
  SWITCH_min_TRIGGERED,
  SWITCH_MAX_TRIGGERED,
  MOTOR_TARGET_REACHED,//followed by specific motor name
  HOMING_COMPLETE,
  HOMING_FAILED
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

motor motorZ1("Z1", A9, A8, 21, 20, 38, 40, maxon_GR, maxon_GR, maxon_MM_PER_ROT, true, false);
motor motorX1("X1", A7, A6, 19, 17, 30, 32, pololu_GR, pololu_CPR, pololu_MM_PER_ROT, true , true);
motor motorY1("Y1", A5, A4, 18, 16, 34, 36, pololu_GR, pololu_CPR, pololu_MM_PER_ROT, true , true);
motor motorY2("Y2", A3, A2, 2, 3, 34, 36, pololu_GR, pololu_CPR, pololu_MM_PER_ROT, true , true);

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
    else if (incomingByte == SET_X_PERCENT + char_offset) {
      int value = read_and_echo_serial();
      double PWM_value = (value / 100.0) * 255.0;
      motorX1.motor_speed = PWM_value;
    }
    else if (incomingByte == SET_Y_PERCENT + char_offset) {
      int value = read_and_echo_serial();
      double PWM_value = (value / 100.0) * 255.0;
      motorY1.motor_speed = PWM_value;
      motorY2.motor_speed = PWM_value;
    }
    else if (incomingByte == SET_Z_PERCENT + char_offset) {
      int value = read_and_echo_serial();
      double PWM_value = (value / 100.0) * 255.0;
      motorZ1.motor_speed = PWM_value;
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
      bool error = homing_sequence();//return fail/success error code here
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

bool homing_sequence() {
  //-----------------------------------------------------
  //x axis homing:
  //find 0:
  motorX1.move_back();
  while (!digitalRead(motorX1.switch_min)) {
    //if any other switches are hit, stop and return error:
    if (digitalRead(motorX1.switch_max) ||
        digitalRead(motorY1.switch_min) ||
        digitalRead(motorY1.switch_max) ||
        digitalRead(motorZ1.switch_min)) {
      motorX1.stop_motor();
      return false;
    }
  }
  //on finding 0, update position:
  motorX1.stop_motor();
  motorX1.quad->write(0);

  //then move away from the limit switch so that it is not triggered:
  motorX1.move_forward();
  while (motorX1.cur_pos < motorX1.buffer_count) {
    motorX1.cur_pos = motorX1.quad->read();
  }

  //find max:
  while (!digitalRead(motorX1.switch_max)) {
    motorX1.cur_pos = motorX1.quad->read();

    //if any other switches are hit, stop and return error:
    if (digitalRead(motorX1.switch_min) ||
        digitalRead(motorY1.switch_max) ||
        digitalRead(motorY1.switch_min) ||
        digitalRead(motorZ1.switch_min)) {
      motorX1.stop_motor();
      return false;
    }

  }
  //on finding max, update position:
  motorX1.stop_motor();
  motorX1.max_pos = motorX1.cur_pos;

  //then move away from the limit switch so that it is not triggered:
  motorX1.move_back();
  while (motorX1.max_pos - motorX1.cur_pos < motorX1.buffer_count) {
    motorX1.cur_pos = motorX1.quad->read();
  }
  motorX1.stop_motor();

  //then set a goal to move back to 0 after homing:
  motorX1.set_goal(0);

  //-----------------------------------------------------
  //y axis homing:

  //find 0:
  motorY1.move_back();
  motorY2.move_back();
  while (!digitalRead(motorY1.switch_min)) {
    //if any other switches are hit, stop and return error:
    if (digitalRead(motorX1.switch_min) ||
        digitalRead(motorX1.switch_max) ||
        digitalRead(motorY1.switch_max) ||
        digitalRead(motorZ1.switch_min)) {
      motorY1.stop_motor();
      motorY2.stop_motor();
      return false;
    }
  }
  //on finding 0, update position:
  motorY1.stop_motor();
  motorY2.stop_motor();
  motorY1.quad->write(0);
  motorY2.quad->write(0);

  //then move away from the limit switch so that it is not triggered:
  motorY1.move_forward();
  motorY2.move_forward();
  while (motorY1.cur_pos < motorY1.buffer_count) {
    motorY1.cur_pos = motorY1.quad->read();
    motorY2.cur_pos = motorY2.quad->read();
  }
  //find max:
  motorY1.move_forward();
  motorY2.move_forward();
  while (!digitalRead(motorY1.switch_max)) {
    motorY1.cur_pos = motorY1.quad->read();
    motorY2.cur_pos = motorY2.quad->read();

    //if any other switches are hit, stop and return error:
    if (digitalRead(motorX1.switch_min) ||
        digitalRead(motorX1.switch_max) ||
        digitalRead(motorY1.switch_min) ||
        digitalRead(motorZ1.switch_min)) {
      motorY1.stop_motor();
      motorY2.stop_motor();
      return false;

    }
  }
  //on finding max, update position:
  motorY1.stop_motor();
  motorY2.stop_motor();
  motorY1.max_pos = motorY1.cur_pos;
  motorY2.max_pos = motorY2.cur_pos;

  //then move away from the limit switch so that it is not triggered:
  motorY1.move_back();
  motorY2.move_back();
  while (motorY1.max_pos - motorY1.cur_pos < motorY1.buffer_count) {
    motorY1.cur_pos = motorY1.quad->read();
    motorY2.cur_pos = motorY2.quad->read();
  }
  motorY1.stop_motor();
  motorY2.stop_motor();

  //then move back to 0 after homing sequence:
  motorY1.set_goal(0);
  motorY2.set_goal(0);
  /*
    //-----------------------------------------------------
    //z axis homing:
  */
  return true;
}
