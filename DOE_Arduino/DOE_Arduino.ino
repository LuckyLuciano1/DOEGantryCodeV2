#include "motors.h"

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
motor motorZ1("Z1", Z1PWM1, Z1PWM2, Z1QUAD1, Z1QUAD2, ZMIN, 0, MAXON353301);
motor motorX1("X1", X1PWM1, X1PWM2, X1QUAD1, X1QUAD2, XMIN, XMAX, POLOLU37D);
motor motorY1("Y1", Y1PWM1, Y1PWM2, Y1QUAD1, Y1QUAD2, YMIN, YMAX, POLOLU37D);
motor motorY2("Y2", Y2PWM1, Y2PWM2, Y2QUAD1, Y2QUAD2, YMIN, YMAX, POLOLU37D);


void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);
}

int serial_status = AWAITING_NEW_COMMAND;
int buffer_data = 0;
bool motors_locked = false;
bool motor_target_given = false;

void loop() {
  delayMicroseconds(1000);
  //update motors:
  if (!motors_locked) {
    if (!motorX1.update_motor() ||
        !motorY1.update_motor() ||
        !motorY2.update_motor() ||
        !motorZ1.update_motor()) {
      //on failure (limit switch hit):
      motorX1.brake();
      motorY1.brake();
      motorY2.brake();
      motorZ1.brake();
      motors_locked = true;
      send_int(MOTORS_LOCKED);
    }
  }
  //if motor target has been given then reached, send to the comupter that the motor movement is successful.
  if (motor_target_given &&
      motorX1.target_reached &&
      motorY1.target_reached &&
      motorY2.target_reached &&
      motorZ1.target_reached) {
    motor_target_given = false;
    send_int(EVENT_SUCCESSFUL);
  }

  delay(1);
  //if not awaiting positional info:
  if (serial_status == AWAITING_NEW_COMMAND &&
      Serial.available() >= 4) { //if any data available

    message m{};//create union to store message

    //read bytes in buffer into message to interpret as integer:
    for (int x = 0; x < 4; x++)//4 is int byte# (default is 2 within arduino
      m.b[x] = Serial.read();

    //interpret data as an ARDUINO_COMMAND:
    switch (m.i) {
      //relays:
      case TURN_ELECTROMAGNETS_OFF:
        digitalWrite(ELECTROMAGNETS, LOW);
        break;
      case TURN_ELECTROMAGNETS_ON:
        digitalWrite(ELECTROMAGNETS, HIGH);
        break;
      case TURN_DRILL_OFF:
        digitalWrite(DRILL, LOW);
        send_int(EVENT_SUCCESSFUL);
        break;
      case TURN_DRILL_ON:
        digitalWrite(DRILL, HIGH);
        send_int(EVENT_SUCCESSFUL);
        break;
      case TURN_FANS_OFF:
        digitalWrite(FANS, LOW);
        break;
      case TURN_FANS_ON:
        digitalWrite(FANS, HIGH);
        break;
      //motor commands:
      case HOME:
        if (homing_sequence())//send error code
          send_int(EVENT_SUCCESSFUL);
        else {
          send_int(EVENT_FAILED);
          exit_and_reset_arduino();
        }
        break;
      case MOVE_X:
        serial_status = GETTING_X_POS;
        break;
      case MOVE_Y:
        serial_status = GETTING_Y_POS;
        break;
      case MOVE_Z:
        serial_status = GETTING_Z_POS;
        break;
      //exit sequence:
      case CLOSE:
        exit_and_reset_arduino();
        break;
    }//switch
  }//awaiting_new_command

  else if (serial_status != AWAITING_NEW_COMMAND && Serial.available() >= 4) {//if next data is float (as of now, everything is):
    //interpret sent data:
    message m{};
    for (int x = 0; x < 4; x++)
      m.b[x] = Serial.read();

    //send value to designated area:
    switch (serial_status) {
      case GETTING_X_POS:
        buffer_data = m.f;                   //store data in buffer for next step
        serial_status = GETTING_X_TARGET_VEL;//next, get vel data
        break;
      case GETTING_Y_POS:
        buffer_data = m.f;
        serial_status = GETTING_Y_TARGET_VEL;
        break;
      case GETTING_Z_POS:
        buffer_data = m.f;
        serial_status = GETTING_Z_TARGET_VEL;
        break;
      case GETTING_X_TARGET_VEL:
        //move motor with sent velocity data and previously sent positional data:
        motorX1.set_target(buffer_data, m.f);
        motor_target_given = true;
        serial_status = AWAITING_NEW_COMMAND;
        break;
      case GETTING_Y_TARGET_VEL:
        motorY1.set_target(buffer_data, m.f);
        motorY2.set_target(buffer_data, m.f);
        motor_target_given = true;
        serial_status = AWAITING_NEW_COMMAND;
        break;
      case GETTING_Z_TARGET_VEL:
        motorZ1.set_target(buffer_data, m.f);
        motor_target_given = true;
        serial_status = AWAITING_NEW_COMMAND;
        break;
    }//switch
  }//not awaiting new command if statement
}//end loop

bool switch_check(int given_switch) {
  //if any other switches besides the given switch are hit, return true, else return false:
  if (digitalRead(XMIN) && given_switch != XMIN) {
    send_int(MIN_X_HIT);
    return true;
  }
  if (digitalRead(XMAX) && given_switch != XMAX) {
    send_int(MAX_X_HIT);
    return true;
  }
  if (digitalRead(YMIN) && given_switch != YMIN) {
    send_int(MIN_Y_HIT);
    return true;
  }
  if (digitalRead(YMAX) && given_switch != YMAX) {
    send_int(MAX_Y_HIT);
    return true;
  }
  if (digitalRead(ZMIN) && given_switch != ZMIN) {
    send_int(MIN_Z_HIT);
    return true;
  }
  return false;
}

bool homing_sequence() {
  const int FULL_POWER = 255;
  //-----------------------------------------------------
  //x axis homing:
  //find 0:
  motorX1.set_PWM(BACK, 255);
  while (!digitalRead(XMIN)) {
    //delay(1);//reading too fast generates false positives on the limit switches for some reason (Y axis specific for, again, some reason)
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
  motorX1.set_PWM(FORWARD, FULL_POWER);
  while (motorX1.current_pos < motorX1.buffer_count) {
    delay(1);
    motorX1.current_pos = motorX1.encoder->read();
  }
  motorX1.encoder->write(0);

  //find max:
  while (!digitalRead(XMAX)) {
    delay(1);
    motorX1.current_pos = motorX1.encoder->read();

    //if any other switches are hit, stop and return error:
    if (switch_check(XMAX)) {
      motorX1.brake();
      return false;
    }
  }
  //on finding max, update position:
  motorX1.brake();
  motorX1.max_pos = motorX1.current_pos;

  //then move away from the limit switch so that it is not triggered at 0:
  motorX1.set_PWM(BACK, FULL_POWER);
  while (motorX1.max_pos - motorX1.current_pos < motorX1.buffer_count) {
    motorX1.current_pos = motorX1.encoder->read();
  }
  motorX1.brake();
  //new max:
  motorX1.max_pos = motorX1.current_pos;

  //-----------------------------------------------------
  //y axis homing:

  //find 0:
  motorY1.set_PWM(BACK, FULL_POWER);
  motorY2.set_PWM(BACK, FULL_POWER);
  while (!digitalRead(YMIN)) {
    delay(1);
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

  //then move away from the limit switch so that it is not triggered at 0:
  motorY1.set_PWM(FORWARD, FULL_POWER);
  motorY2.set_PWM(FORWARD, FULL_POWER);
  
  while (motorY1.current_pos < motorY1.buffer_count) {
    motorY1.current_pos = motorY1.encoder->read();
    motorY2.current_pos = motorY2.encoder->read();
  }
  //new 0:
  motorY1.encoder->write(0);
  motorY2.encoder->write(0);

  //find max:
  motorY1.set_PWM(FORWARD, FULL_POWER);
  motorY2.set_PWM(FORWARD, FULL_POWER);

  while (!digitalRead(YMAX)) {
    delay(1);
    motorY1.current_pos = motorY1.encoder->read();
    motorY2.current_pos = motorY2.encoder->read();

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
  motorY1.max_pos = motorY1.current_pos;
  motorY2.max_pos = motorY2.current_pos;

  //then move away from the limit switch so that it is not triggered:
  motorY1.set_PWM(BACK, FULL_POWER);
  motorY2.set_PWM(BACK, FULL_POWER);
  while (motorY1.max_pos - motorY1.current_pos < motorY1.buffer_count) {
    motorY1.current_pos = motorY1.encoder->read();
    motorY2.current_pos = motorY2.encoder->read();
  }

  motorY1.brake();
  motorY2.brake();
  //new max:
  motorY1.max_pos = motorY1.current_pos;
  motorY2.max_pos = motorY2.current_pos;

  //then move back to 0 after homing sequence:
  //motorX1.set_target(0, GANTRY_FAST_TRAVEL);
  //motorY1.set_target(0, GANTRY_FAST_TRAVEL);
  //motorY2.set_target(0, GANTRY_FAST_TRAVEL);

  //-----------------------------------------------------
  //z axis homing:

  return true;
}

void exit_and_reset_arduino()
{
  motorX1.brake();
  motorY1.brake();
  motorY2.brake();
  motorZ1.brake();
  digitalWrite(FANS, LOW);
  digitalWrite(DRILL, LOW);
  digitalWrite(ELECTROMAGNETS, LOW);
  reset_arduino();
}
