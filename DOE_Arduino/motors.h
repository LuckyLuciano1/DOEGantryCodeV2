class motor {

  private:

    const double GR = 30;//gear ratio
    const double CPR = 64;//encoder counts per rotation
    const double MM_PER_ROT = 5;

    int in1, in2, quad1, quad2;
    Encoder *quad;

    double cur_pos = 0;
    double des_pos = 0;
    const int overshoot = 192;//when within 384 count (aka 0.5mm), stop
    double acceleration = 0;
    bool dir = true; //T = forward, F = backward
    bool target_reached = true;

  public:

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
    void set_relative_goal(double diff_mm) {
      double goal_mm = des_pos + diff_mm;
      double rotations = goal_mm / MM_PER_ROT;
      des_pos = rotations * GR * CPR;

      target_reached = false;
    }

    void update_motor()
    {
      if (!target_reached) {
        cur_pos = quad->read();//update current position

        if (cur_pos + overshoot < des_pos) {
          //slow down if switching direction:
          if (dir == false) //aka was moving backward last cycle
          {
            dir = true;
            acceleration = 0;
            analogWrite(in1, 0);
          }
          if (acceleration < 255)
            acceleration += 0.125;

          analogWrite(in2, 255);
        }
        else if (cur_pos - overshoot > des_pos) {
          //slow down if switching direction:
          if (dir == true) //aka was moving forward last cycle
          {
            dir = false;
            acceleration = 0;
            analogWrite(in2, 0);
          }
          //
          if (acceleration < 255)
            acceleration += 0.125;

          analogWrite(in1, 255);
        }
        else { //near target - slow down
          acceleration--;
          if (acceleration == 0)
            target_reached = true;
        }
      }

    }
    void close_motor(){
      analogWrite(in1, 0);
      analogWrite(in2, 0);
    }
    int get_goal() {
      return des_pos;
    }
};
