class motor {
  private:
    const double GR = 30;//gear ratio
    const double CPR = 64;//encoder counts per rotation
    const double MM_PER_ROT = 5;
    const int overshoot = 384*1.7;//when within 384 count (aka 0.5mm), stop

    int in1, in2, quad1, quad2;

    long cur_pos = 0;
    long des_pos = 0;
    bool target_reached = true;

  public:
    Encoder *quad;

    motor(int in1, int in2, int quad1, int quad2)
    {
      this->in1 = in1;
      this->in2 = in2;
      this->quad1 = quad1;
      this->quad2 = quad2;

      pinMode(in1, OUTPUT);
      pinMode(in2, OUTPUT);
      quad = new Encoder(quad1, quad2);
    }

    void set_goal(double goal_mm) { //from range of (ex) 0 to 1500mm
      double rotations = goal_mm / MM_PER_ROT;
      des_pos = rotations * GR * CPR;
      target_reached = false;
    }

    void update_motor() {

     cur_pos = quad->read();//update current position

     if (!target_reached) {//move towards target

        if (cur_pos +overshoot < des_pos) {
          digitalWrite(in1, LOW);
          digitalWrite(in2, HIGH);
        }
        else if (cur_pos - overshoot > des_pos) {
          digitalWrite(in2, LOW);
          digitalWrite(in1, HIGH);
        }
        else { //near target - stop
          target_reached = true;
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
        }
      }
    }
    void close_motor() {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }
    int get_goal() {
      return des_pos;
    }
    long get_pos() {
      return cur_pos;
    }
};
