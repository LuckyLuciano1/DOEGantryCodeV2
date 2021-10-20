#define zmin 30
#define ymax 32
#define ymin 34
#define xmax 36
#define xmin 38

void setup() {
  Serial.begin(9600);
  pinMode(xmin, INPUT_PULLUP);
  pinMode(xmax, INPUT_PULLUP);
  pinMode(ymin, INPUT_PULLUP);
  pinMode(ymax, INPUT_PULLUP);
  pinMode(zmin, INPUT_PULLUP);
}

bool xminflag, xmaxflag, yminflag, ymaxflag, zminflag, zmaxflag;
void loop() {
  xminflag = digitalRead(xmin);
  xmaxflag = digitalRead(xmax);
  yminflag = digitalRead(ymin);
  ymaxflag = digitalRead(ymax);
  zminflag = digitalRead(zmin);

  if (xminflag)
    Serial.print("XMIN");

  if (xmaxflag)
    Serial.print("XMAX");

  if (yminflag)
    Serial.print("YMIN");

  if (ymaxflag)
    Serial.print("YMAX");

  if (zminflag)
    Serial.print("ZMIN");

  Serial.println(";");
  //delay(1);
}
