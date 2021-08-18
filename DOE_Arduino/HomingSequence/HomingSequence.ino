#include "motors.h"

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

//motor specs:
int pololu_CPR = 64;
int pololu_GR = 30;
int pololu_MM_PER_ROT = 5;
int maxon_CPR = 500;
int maxon_GR = 308;
int maxon_MM_PER_ROT = 93;

motor motorZ1("Z1", Z1PWM1, Z1PWM2, Z1QUAD1, Z1QUAD2, ZMIN, NULL, maxon_GR, maxon_GR, maxon_MM_PER_ROT, true, false);
motor motorX1("X1", X1PWM1, X1PWM2, X1QUAD1, X1QUAD2, XMIN, XMAX, pololu_GR, pololu_CPR, pololu_MM_PER_ROT, true , true);
motor motorY1("Y1", Y1PWM1, Y1PWM2, Y1QUAD1, Y1QUAD2, YMIN, YMAX, pololu_GR, pololu_CPR, pololu_MM_PER_ROT, true , true);
motor motorY2("Y2", Y2PWM1, Y2PWM2, Y2QUAD1, Y2QUAD2, YMIN, YMAX, pololu_GR, pololu_CPR, pololu_MM_PER_ROT, true , true);

void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);

  pinMode(ERR_PIN, OUTPUT);

  //HOMING:
  bool error = homing_sequence();//return fail/success error code here
  if (!error)
    digitalWrite(ERR_PIN, HIGH);
}
void print_motor() {
  Serial.print("X1: ");
  Serial.print(motorX1.quad->read());
  Serial.print(" - Y1: ");
  Serial.print(motorY1.quad->read());
  Serial.print(" - Y2: ");
  Serial.println(motorY2.quad->read());
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
  motorX1.move_back();
  while (!digitalRead(XMIN)) {
    //if any other switches are hit, stop and return error:
    print_motor();
    if (switch_check(XMIN)) {
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
    print_motor();
    motorX1.cur_pos = motorX1.quad->read();
  }

  //find max:
  while (!digitalRead(XMAX)) {
    motorX1.cur_pos = motorX1.quad->read();
    print_motor();
    //if any other switches are hit, stop and return error:
    if (switch_check(XMAX)) {
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
    print_motor();
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
  while (!digitalRead(YMIN)) {
    print_motor();
    //if any other switches are hit, stop and return error:
    if (switch_check(YMIN)) {
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
  //delay(125);
  while (!digitalRead(YMAX)) {
    print_motor();
    motorY1.cur_pos = motorY1.quad->read();
    motorY2.cur_pos = motorY2.quad->read();

    //if any other switches are hit, stop and return error:
    if (switch_check(YMAX)) {
      motorY1.stop_motor();
      motorY2.stop_motor();
      return false;
    }
  }
  //on finding max, update position:
  motorY1.stop_motor();
  motorY2.stop_motor();
  return false;
  motorY1.max_pos = motorY1.cur_pos;
  motorY2.max_pos = motorY2.cur_pos;

  //then move away from the limit switch so that it is not triggered:
  motorY1.move_back();
  motorY2.move_back();
  while (motorY1.max_pos - motorY1.cur_pos < motorY1.buffer_count) {
    print_motor();
    motorY1.cur_pos = motorY1.quad->read();
    motorY2.cur_pos = motorY2.quad->read();
  }
  motorY1.stop_motor();
  motorY2.stop_motor();

  //then move back to 0 after homing sequence:
  motorY1.set_goal(0);
  motorY2.set_goal(0);

  //-----------------------------------------------------
  //z axis homing:

  return true;
}

void loop() {
  motorX1.update_motor();
  motorY1.update_motor();
  motorY2.update_motor();
  motorZ1.update_motor();
  print_motor();
}
