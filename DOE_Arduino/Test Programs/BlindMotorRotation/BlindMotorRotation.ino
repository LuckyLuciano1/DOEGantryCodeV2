#include "Ticker.h"
#define IN1 6
#define IN2 7

void printI();
Ticker timer1(printI, 5);//every 5 ms
int i = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  //start in off:
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
}

void loop() {
  timer1.update();

  int rate = 5;
  for (i = 0; i < 255; i++) { //forward
    analogWrite(IN1, 0);
    analogWrite(IN2, i);
    delay(rate);
  }
  for (i = 255; i > 0; i--) {
    analogWrite(IN1, 0);
    analogWrite(IN2, i);
    delay(rate);
  }

   for (i = 0; i < 255; i++) { //backward
    analogWrite(IN2, 0);
    analogWrite(IN1, i);
    delay(rate);
  }
  for (i = 255; i > 0; i--) {
    analogWrite(IN2, 0);
    analogWrite(IN1, i);
    delay(rate);
  }
}

void printI() {
  Serial.println(i);
}
