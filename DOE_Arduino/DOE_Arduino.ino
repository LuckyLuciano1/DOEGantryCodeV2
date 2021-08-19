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
  SWITCH_MIN_TRIGGERED,
  SWITCH_MAX_TRIGGERED,
  MOTOR_TARGET_REACHED,//followed by specific motor name
  HOMING_COMPLETE,
  HOMING_FAILED
};

//pin assignment:
#define ELECTROMAGNETS 44
#define DRILL 42
#define FANS 40

//motor pins:
#define Z1PWM1 4
#define Z1PWM2 5
#define X1PWM1 6
#define X1PWM2 7
#define Y1PWM1 8
#define Y1PWM2 9
#define Y2PWM1 10
#define Y2PWM2 11

#define Y2QUAD1 2
#define Y2QUAD2 3
#define X1QUAD1 18
#define X1QUAD2 16
#define Y1QUAD1 19
#define Y1QUAD2 17
#define Z1QUAD1 21
#define Z1QUAD2 20

#define XMIN 38
#define XMAX 36
#define YMIN 34
#define YMAX 32
#define ZMIN 30

//motor initialization:
motor motorZ1("Z1", Z1PWM1, Z1PWM2, Z1QUAD1, Z1QUAD2, ZMIN, NULL, MAXON353301);
motor motorX1("X1", X1PWM1, X1PWM2, X1QUAD1, X1QUAD2, XMIN, XMAX, POLOLU37D);
motor motorY1("Y1", Y1PWM1, Y1PWM2, Y1QUAD1, Y1QUAD2, YMIN, YMAX, POLOLU37D);
motor motorY2("Y2", Y2PWM1, Y2PWM2, Y2QUAD1, Y2QUAD2, YMIN, YMAX, POLOLU37D);


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
      motorX1.set_target(value, GANTRY_FAST_TRAVEL);//later modify to get velocity
    }
    else if (incomingByte == MOVE_Y + char_offset) {
      int value = read_and_echo_serial();
      motorY1.set_target(value, GANTRY_FAST_TRAVEL);
      motorY2.set_target(value, GANTRY_FAST_TRAVEL);
    }
    else if (incomingByte == MOVE_Z + char_offset) {
      int value = read_and_echo_serial();
      motorZ1.set_target(value, GANTRY_FAST_TRAVEL);
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
      motorX1.brake();
      motorY1.brake();
      motorY2.brake();
      motorZ1.brake();
      digitalWrite(FANS, LOW);
      digitalWrite(DRILL, LOW);
      digitalWrite(ELECTROMAGNETS, LOW);
    }
    else if (incomingByte == HOME + char_offset) {
      bool error = homing_sequence();//return fail/success error code here
      //if (!error)
      //  digitalWrite(ERR_PIN, HIGH);
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

bool switch_check(int given_switch) {
  //if any other switches besides the given switch are hit, return true, else return false:
  if (digitalRead(XMIN) && given_switch != XMIN)
    return true;
  if (digitalRead(XMAX) && given_switch != XMAX)
    return true;
  if (digitalRead(YMIN) && given_switch != YMIN)
    return true;
  if (digitalRead(YMAX) && given_switch != YMAX)
    return true;
  if (digitalRead(ZMIN) && given_switch != ZMIN)
    return true;

  return false;
}

bool homing_sequence() {
  //-----------------------------------------------------
  //x axis homing:
  //find 0:
  motorX1.set_PWM(BACK, 255);
  while (!digitalRead(XMIN)) {
    //if any other switches are hit, stop and return error:
    if (switch_check(XMIN)) {
      motorX1.brake();
      return false;
    }
  }
  //on finding 0, update position:
  motorX1.brake();
  motorX1.encoder->write(0);

  //then move away from the limit switch so that it is not triggered:
  motorX1.set_PWM(FORWARD, 255);
  while (motorX1.cur_pos < motorX1.buffer_count) {
    motorX1.cur_pos = motorX1.encoder->read();
  }

  //find max:
  while (!digitalRead(XMAX)) {
    motorX1.cur_pos = motorX1.encoder->read();

    //if any other switches are hit, stop and return error:
    if (switch_check(XMAX)) {
      motorX1.brake();
      return false;
    }
  }
  //on finding max, update position:
  motorX1.brake();
  motorX1.max_pos = motorX1.cur_pos;

  //then move away from the limit switch so that it is not triggered:
  motorX1.set_PWM(BACK, 255);
  while (motorX1.max_pos - motorX1.cur_pos < motorX1.buffer_count) {
    motorX1.cur_pos = motorX1.encoder->read();
  }
  motorX1.brake();

  //then set a goal to move back to 0 after homing:
  //motorX1.set_target(0, GANTRY_FAST_TRAVEL);
  delay(250);
  //-----------------------------------------------------
  //y axis homing:

  //find 0:
  motorY1.set_PWM(BACK, 255);
  motorY2.set_PWM(BACK, 255);
  while (!digitalRead(YMIN)) {
    //if any other switches are hit, stop and return error:
    if (switch_check(YMIN)) {
      motorY1.brake();
      motorY2.brake();
      return false;
    }
  }
  //on finding 0, update position:
  motorY1.brake();
  motorY2.brake();
  motorY1.encoder->write(0);
  motorY2.encoder->write(0);

  //then move away from the limit switch so that it is not triggered:
  motorY1.set_PWM(FORWARD, 255);
  motorY2.set_PWM(FORWARD, 255);
  while (motorY1.cur_pos < motorY1.buffer_count) {
    motorY1.cur_pos = motorY1.encoder->read();
    motorY2.cur_pos = motorY2.encoder->read();
  }

  //find max:
  motorY1.set_PWM(FORWARD, 255);
  motorY2.set_PWM(FORWARD, 255);

  while (!digitalRead(YMAX)) {
    motorY1.cur_pos = motorY1.encoder->read();
    motorY2.cur_pos = motorY2.encoder->read();

    //if any other switches are hit, stop and return error:
    if (switch_check(YMAX)) {
      motorY1.brake();
      motorY2.brake();
      return false;
    }
  }
  //on finding max, update position:
  motorY1.brake();
  motorY2.brake();
  motorY1.max_pos = motorY1.cur_pos;
  motorY2.max_pos = motorY2.cur_pos;

  //then move away from the limit switch so that it is not triggered:
  motorY1.set_PWM(BACK, 255);
  motorY2.set_PWM(BACK, 255);
  while (motorY1.max_pos - motorY1.cur_pos < motorY1.buffer_count) {
    motorY1.cur_pos = motorY1.encoder->read();
    motorY2.cur_pos = motorY2.encoder->read();
  }
  motorY1.brake();
  motorY2.brake();
  delay(250);
  //then move back to 0 after homing sequence:
  //motorX1.set_target(0, GANTRY_FAST_TRAVEL);
  //motorY1.set_target(0, GANTRY_FAST_TRAVEL);
  //motorY2.set_target(0, GANTRY_FAST_TRAVEL);

  //-----------------------------------------------------
  //z axis homing:

  return true;
}
