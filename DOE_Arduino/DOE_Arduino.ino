#include <Encoder.h>
#include "motors.h"

enum ARDUINO_COMMANDS {
  TURN_ELECTROMAGNETS_OFF,
  TURN_ELECTROMAGNETS_ON,
  TURN_DRILL_OFF,
  TURN_DRILL_ON,
  TURN_FANS_OFF,
  TURN_FANS_ON
};

#define ELECTROMAGNETS 5
#define DRILL 6
#define FANS 7

//motor motor1(8, 9, 8, 9, 1, 4);
//motor motor2(8, 9, 8, 9, 1, 4);
//motor motor3(8, 9, 8, 9, 1, 4);

void setup() {
  Serial.begin(9600);
  pinMode(ELECTROMAGNETS, OUTPUT);
  pinMode(DRILL, OUTPUT);
  pinMode(FANS, OUTPUT);
}

void loop() {
  //update morors:

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
  }
}
