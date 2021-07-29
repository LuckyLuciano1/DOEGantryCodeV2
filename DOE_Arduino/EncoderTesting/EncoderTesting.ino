#include <Encoder.h>

//#define Z1Q1 21
//#define Z1Q2 20
#define X1Q1 18
#define X1Q2 16
#define Y1Q1 19
#define Y1Q2 17
#define Y2Q1 2
#define Y2Q2 3

void setup() {
  Serial.begin(9600);
}
//Encoder *quadZ1 = new Encoder(X1Q1, X1Q2);
Encoder *quadX1 = new Encoder(X1Q1, X1Q2);
Encoder *quadY1 = new Encoder(Y1Q1, Y1Q2);
Encoder *quadY2 = new Encoder(Y2Q1, Y2Q2);

void loop() {
  //Serial.print("Z1: ");
  //Serial.println(quadZ1->read());
  Serial.print("X1: ");
  Serial.print(quadX1->read());
  Serial.print(" - Y1: ");
  Serial.print(quadY1->read());
  Serial.print(" - Y2: ");
  Serial.println(quadY2->read());
}
