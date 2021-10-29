#include "motors.h"
#include <Robojax_AllegroACS_Current_Sensor.h>

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
#define Z1QUAD1 20
#define Z1QUAD2 21

#define XMIN 38
#define XMAX 36
#define YMIN 34
#define YMAX 32
#define ZMIN 30

const int CURRENTPIN = A0;

//switch & motor initialization:
limit_switch switchXmin(XMIN);
limit_switch switchXmax(XMAX);
limit_switch switchYmin(YMIN);
limit_switch switchYmax(YMAX);
limit_switch switchZmax(ZMIN);

motor motorZ1("Z1", Z1PWM1, Z1PWM2, Z1QUAD1, Z1QUAD2, &switchZmax, NULL, MAXON353301);
motor motorX1("X1", X1PWM1, X1PWM2, X1QUAD1, X1QUAD2, &switchXmin, &switchXmax, POLOLU37D);
motor motorY1("Y1", Y1PWM1, Y1PWM2, Y1QUAD1, Y1QUAD2, &switchYmin, &switchYmax, POLOLU37D);
motor motorY2("Y2", Y2PWM1, Y2PWM2, Y2QUAD1, Y2QUAD2, &switchYmin, &switchYmax, POLOLU37D);

Robojax_AllegroACS_Current_Sensor current_sensor(1, CURRENTPIN); //our sensor is model 1 (AMP range of 20)

void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);
  //motorX1.set_target(25.0f, 60.0f);
}

int serial_status = AWAITING_NEW_COMMAND;
float buffer_data = 0.0f;
bool motors_locked = false;
bool motor_target_given = false;

void loop() {
  //delayMicroseconds(1000);

  update_switches();
  //MOTORS:
  if (!motors_locked) {
    if (!motorX1.update() ||
        !motorY1.update() ||
        !motorY2.update() ||
        !motorZ1.update()) {
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

  //COMMUNICATION:

  //if not awaiting positional info:
  if (serial_status == AWAITING_NEW_COMMAND &&
      Serial.available() >= 4) { //if any data available

    message m{};//create union to store message

    //read bytes in buffer into message to interpret as integer:
    for (int x = 0; x < 4; x++)//4 is int byte# (default is 2 within arduino, which is incorrect)
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
        delay(1);
        send_int(EXIT_AND_RESET);
        delay(1);
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

void update_switches() {
  switchXmin.update();
  switchXmax.update();
  switchYmin.update();
  switchYmax.update();
  switchZmax.update();
}

bool switch_check(int given_switch) {
  update_switches();

  //if any other switches besides the given switch are hit, return true, else return false:
  if (switchXmin.read() && given_switch != XMIN) {
    send_int(MIN_X_HIT);
    return true;
  }
  if (switchXmax.read() && given_switch != XMAX) {
    send_int(MAX_X_HIT);
    return true;
  }
  if (switchYmin.read() && given_switch != YMIN) {
    send_int(MIN_Y_HIT);
    return true;
  }
  if (switchYmax.read() && given_switch != YMAX) {
    send_int(MAX_Y_HIT);
    return true;
  }
  if (switchZmax.read() && given_switch != ZMIN) {
    send_int(MIN_Z_HIT);
    return true;
  }
  return false;
}

bool homing_sequence() {
  const int FULL_POWER = 255;
  const int DELAY_TIME = 0;
  //-----------------------------------------------------
  //x axis homing:
  //find 0:
  motorX1.set_PWM(BACK, 255);
  while (!switchXmin.read()) {
    delay(DELAY_TIME);//reading too fast generates false positives on the limit switches for some reason (Y axis specific for, again, some reason)
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
    delay(DELAY_TIME);
    motorX1.current_pos = motorX1.encoder->read();
  }
  motorX1.encoder->write(0);
  motorX1.brake();
  update_switches();
  
  //-----------------------------------------------------
  //y axis homing:

  //find 0:
  motorY1.set_PWM(BACK, FULL_POWER);
  motorY2.set_PWM(BACK, FULL_POWER);
  while (!switchYmin.read()) {
    delay(DELAY_TIME);
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
    delay(DELAY_TIME);
    motorY1.current_pos = motorY1.encoder->read();
    motorY2.current_pos = motorY2.encoder->read();
  }
  //new 0:
  motorY1.encoder->write(0);
  motorY2.encoder->write(0);
  motorY1.brake();
  motorY2.brake();
  update_switches();

//-----------------------------------------------------
  //z axis homing:
  //find 0:
  motorZ1.set_PWM(FORWARD, FULL_POWER);//for Z, forward moves down and -- encoder value
  //Serial.println(current_sensor.getCurrentAverage(300));
  current_sensor.getCurrentAverage(300);//first averaged value seems to give issues
  while (current_sensor.getCurrentAverage(300) < 4.0f) {
    delay(DELAY_TIME);
    //Serial.println(current_sensor.getCurrentAverage(300));
    motorZ1.current_pos = motorZ1.encoder->read();
    //if any other switches are hit, stop and return error:
    if (switchZmax.read()) {
      motorZ1.brake();
      return false;
    }
  }
  //on finding max, update position:
  motorZ1.brake();
  motorZ1.encoder->write(0);
  motorZ1.current_pos = motorZ1.encoder->read();
  update_switches();
  
  //then move away from the limit switch so that it is not triggered:(*5 so that it is not too close to the plate)
  motorZ1.set_PWM(BACK, FULL_POWER);
  
  //move back:
  while (motorZ1.current_pos < motorZ1.buffer_count*5.0f) {
    delay(DELAY_TIME);
    motorZ1.current_pos = motorZ1.encoder->read();
  }
  
  /*while (motorZ1.max_pos - motorZ1.current_pos < motorZ1.buffer_count * 5.0f) {
    delay(DELAY_TIME);
    motorZ1.current_pos = motorZ1.encoder->read();
  }*/

  motorZ1.brake();
  update_switches();
  //homing complete
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
