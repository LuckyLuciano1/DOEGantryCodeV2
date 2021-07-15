#include <Encoder.h>
#include "motors.h"

enum ARDUINO_COMMANDS {
  TURN_ELECTROMAGNETS_OFF,
  TURN_ELECTROMAGNETS_ON,
  TURN_DRILL_OFF,
  TURN_DRILL_ON,
  TURN_FANS_OFF,
  TURN_FANS_ON,
  MOVE_MOTOR,
  CLOSE
};

#define ELECTROMAGNETS 5
#define DRILL 6
#define FANS 7

motor motor1(A4, A5, 2, 3);
motor motor2(A2, A3, 19, 18);
motor motor3(A0, A1, 21, 20);

void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);
}
int motor_loop = 0;

void loop() {
  //update motors:
  motor1.update_motor();
  motor2.update_motor();
  motor3.update_motor();

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
    else if (incomingByte == MOVE_MOTOR + char_offset) {
      motor_loop++;
      if (motor_loop == 1)
        motor1.set_goal(10);
      else if (motor_loop == 2)
        motor1.set_goal(0);
      else if (motor_loop == 3)
        motor1.set_goal(15);
      else if (motor_loop == 4)
        motor1.set_goal(0);
    }
    else if (incomingByte == CLOSE + char_offset) {
      digitalWrite(FANS, LOW);
      digitalWrite(DRILL, LOW);
      digitalWrite(ELECTROMAGNETS, LOW);
      motor1.close_motor();
      motor2.close_motor();
      motor3.close_motor();
    }
  }
}
