#include <Encoder.h>
#include "motors.h"

enum ARDUINO_COMMANDS {
  TURN_ELECTROMAGNETS_OFF,
  TURN_ELECTROMAGNETS_ON,
  TURN_DRILL_OFF,
  TURN_DRILL_ON,
  TURN_FANS_OFF,
  TURN_FANS_ON,
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  CLOSE
};

#define ELECTROMAGNETS 5
#define DRILL 6
#define FANS 7

int pololu_CPR = 64;
int pololu_GR = 30;
int pololu_MM_PER_ROT = 5;
int maxon_CPR = 512;
int maxon_GR = 19;//temp
int maxon_MM_PER_ROT = 5;//temp

motor motorX1(A5, A4, 2, 3, pololu_GR, pololu_CPR, pololu_MM_PER_ROT);
motor motorY1(A3, A2, 16, 19, pololu_GR, pololu_CPR, pololu_MM_PER_ROT);
motor motorY2(A1, A0, 17, 21, pololu_GR, pololu_CPR, pololu_MM_PER_ROT);
motor motorZ1(A6, A7, 18, 20, maxon_GR, maxon_GR, maxon_MM_PER_ROT);

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
      motorX1(value);
    }
    else if (incomingByte == MOVE_Y + char_offset) {
      int value = read_and_echo_serial();
      motorY1(value);
      motorY2(value);
    }
    else if (incomingByte == MOVE_Z + char_offset) {
      int value = read_and_echo_serial();
      motorZ1(value);
    }
    else if (incomingByte == CLOSE + char_offset) {
      digitalWrite(FANS, LOW);
      digitalWrite(DRILL, LOW);
      digitalWrite(ELECTROMAGNETS, LOW);
      motorX1.close_motor();
      motorY1.close_motor();
      motorY2.close_motor();
      motorZ1.close_motor();
    }
  }
}

int read_values() {
  char incomingByte = Serial.read();
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
  motor1.set_goal(value);
}
