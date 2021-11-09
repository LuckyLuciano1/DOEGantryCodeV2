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
#define Y1PWM2 9//may be issue here
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

Robojax_AllegroACS_Current_Sensor current_sensor(1, CURRENTPIN); //our sensor is model 2

void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);
  Serial.print("//////////////////////////////////////");
  digitalWrite(ELECTROMAGNETS, HIGH);
  homing_sequence();
  digitalWrite(ELECTROMAGNETS, LOW);
}

int serial_status = AWAITING_NEW_COMMAND;
float buffer_data = 0.0f;
bool motors_locked = false;
bool motor_target_given = false;

int counter = 0;
bool flag = true;

void loop() {

  if (flag) {
    Serial.println("SETTING TARGET");
    switch (counter) {
      case 0:
        motorX1.set_target(50.0f, 100.0f);//cm_per_min
        motor_target_given = true;
        break;
      case 1:
        motorX1.set_target(1.0f, 100.0f);
        motor_target_given = true;
        break;
    }
    flag = false;
  }

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
  
  /*Serial.print(motor_target_given);
  Serial.print(motorX1.target_reached);
  Serial.print(motorY1.target_reached);
  Serial.print(motorY2.target_reached);
  Serial.println(motorZ1.target_reached);*/
  
  //if motor target has been given then reached, send to the comupter that the motor movement is successful.
  if (motor_target_given &&
      motorX1.target_reached &&
      motorY1.target_reached &&
      motorY2.target_reached &&
      motorZ1.target_reached) {
    motor_target_given = false;
    counter++;
    flag = true;
    Serial.println("motor event complete");
    //send_int(EVENT_SUCCESSFUL);
  }
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
    //reading too fast generates false positives on the limit switches for some reason (Y axis specific for, again, some reason)
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
  motorY1.brake();
  motorY2.brake();
  update_switches();

  //-----------------------------------------------------
  //z axis homing:
  //find 0:
  /*
    motorZ1.set_PWM(FORWARD, FULL_POWER);//for Z, forward moves down and subtracts from encoder value
    delay(20);//current spikes in the first few moments of motor running, so don't measure just yet
    float current_value = current_sensor.getCurrentAverage(300);
    while (current_value < 4.01f) {
    delay(20);
    current_value = current_sensor.getCurrentAverage(300);
    motorZ1.current_pos = motorZ1.encoder->read();
    //if any other switches are hit, stop and return error:
    if (switchZmax.read()) {
      motorZ1.brake();
      return false;
    }
    }
    //update position:
    motorZ1.brake();
    motorZ1.encoder->write(0);
    motorZ1.current_pos = motorZ1.encoder->read();
    update_switches();

    //then move away from the limit switch so that it is not triggered:(*5 so that it is not too close to the plate)
    motorZ1.set_PWM(BACK, FULL_POWER);

    //move back:
    while (motorZ1.current_pos < motorZ1.buffer_count*5.0f) {
    motorZ1.current_pos = motorZ1.encoder->read();
    }
    motorZ1.brake();
    update_switches();
  */
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
