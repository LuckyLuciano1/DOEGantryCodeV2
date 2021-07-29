#define xmin 30
#define xmax 32
#define ymin 34
#define ymax 36
#define zmin 38

void setup() {
  // put your setup code here, to run once:
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
}
