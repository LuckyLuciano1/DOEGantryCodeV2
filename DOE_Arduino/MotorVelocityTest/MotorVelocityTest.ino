//#include "motors.h"
#include "Encoder.h"
#include "Ticker.h"

#define IN1 11
#define IN2 12
#define QUAD1 3
#define QUAD2 2

Encoder *quad;

void CalculateVel();
Ticker timer1(CalculateVel, 5);//every 5 ms
float pos = 0;
float old_pos = 0;

int motor_dir = 1;
float motor_speed = 0;
bool accelerating = true;

void setup() {
  Serial.begin(9600);

  quad = new Encoder(QUAD1, QUAD2);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  //start in off:
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  timer1.start();
}

void loop() {
  timer1.update();
  MotorLogic();
  delay(5);
}
void MotorLogic() {
  if (accelerating)
    motor_speed += .5;
  else
    motor_speed -= .5;

  if (motor_dir == 1) { //moving forward
    analogWrite(IN1, LOW);
    analogWrite(IN2, motor_speed);
  }
  else {                //moving backward
    analogWrite(IN2, LOW);
    analogWrite(IN1, motor_speed);
  }
Serial.println(motor_speed);
  //control logic:
  if (accelerating && motor_speed >= 255)//if motor has reached peak velocity,
    Serial.println("slowing"), accelerating = false;                //begin slowing down
  
  else if (motor_dir == 1 && !accelerating && motor_speed <= 0) //if motor has slowed to a stop
    Serial.println("CW Reverse"), accelerating = true, motor_dir = -1;                   //reverse direction and speed up

  else if (motor_dir == -1 && !accelerating && motor_speed <= 0) //if motor has slowed to a stop
    Serial.println("CCW, Reverse"), accelerating = true, motor_dir = 1;                     //reverse direction and speed up


}
void CalculateVel() {
  pos = quad->read();
  float change = pos - old_pos;
  float vel = change / 5; //5 ms
  //Serial.println(vel);
  old_pos = pos;
}
