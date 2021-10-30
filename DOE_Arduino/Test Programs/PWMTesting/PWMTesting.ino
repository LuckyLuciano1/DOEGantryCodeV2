#include <Encoder.h>

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

enum DIRECTION {
  BACK,
  FORWARD
};

void setup() {
  Serial.begin(9600);
  pinMode(Z1PWM1, OUTPUT);
  pinMode(Z1PWM2, OUTPUT);
  pinMode(X1PWM1, OUTPUT);
  pinMode(X1PWM2, OUTPUT);
  pinMode(Y1PWM1, OUTPUT);
  pinMode(Y1PWM2, OUTPUT);
  pinMode(Y2PWM1, OUTPUT);
  pinMode(Y2PWM2, OUTPUT);

  //start motor in off position:
  digitalWrite(Z1PWM1, LOW);
  digitalWrite(Z1PWM2, LOW);
  digitalWrite(X1PWM1, LOW);
  digitalWrite(X1PWM2, LOW);
  digitalWrite(Y1PWM1, LOW);
  digitalWrite(Y1PWM2, LOW);
  digitalWrite(Y2PWM1, LOW);
  digitalWrite(Y2PWM2, LOW);

  Encoder *Z1enc = new Encoder(Z1QUAD1, Z1QUAD2);
  Z1enc->write(1);
  Encoder *X1enc = new Encoder(X1QUAD1, X1QUAD2);
  X1enc->write(1);
  Encoder *Y1enc = new Encoder(Y1QUAD1, Y1QUAD2);
  Y1enc->write(1);
  Encoder *Y2enc = new Encoder(Y2QUAD1, Y2QUAD2);
  Y2enc->write(1);
      
  //demo:
 

  //back_and_forth(X1PWM1, X1PWM2);
  //back_and_forth(Y1PWM1, Y1PWM2);
  //back_and_forth(Y2PWM1, Y2PWM2);
  set_PWM(Z1PWM1, Z1PWM2, BACK, 255);
  for(int x = 0; x < 300; x++){
    delay(1);
    Serial.println(Z1enc->read());
  }
  //delay(500);
  brake(Z1PWM1, Z1PWM2);
}

void loop() {}


void back_and_forth(int PWM1, int PWM2) {
  int FULL = 255;
  set_PWM(PWM1, PWM2, FORWARD, FULL); 
  delay(2000);
  set_PWM(PWM1, PWM2, BACK, FULL);
  delay(2000);
  brake(PWM1, PWM2);
  delay(1000);
}

void set_PWM(int PWM1, int PWM2, bool dir, uint8_t pwm) {
  if (dir == FORWARD) {    //left
    analogWrite(PWM2, 0);
    delayMicroseconds(100);
    analogWrite(PWM1, pwm);
  }
  else if (dir == BACK) {  //right
    analogWrite(PWM1, 0);
    delayMicroseconds(100);
    analogWrite(PWM2, pwm);
  }
}

void brake(int PWM1, int PWM2) {
  analogWrite(PWM1, LOW);
  analogWrite(PWM2, LOW);
}
