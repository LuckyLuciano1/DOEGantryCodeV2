class motor {
  private:
    double GR;//gear ratio
    double CPR;//encoder counts per rotation
    double MM_PER_ROT = 5;
    const int overshoot = 384 * 1.7; //when within 384 count (aka 0.5mm), stop (1.7 is a tuning value)

    int in1, in2, quad1, quad2;
    const char* label;
    bool target_reached = true;

  public:
    long cur_pos = 0;
    long des_pos = 0;
    long max_pos = 0;
    Encoder *quad;
    int switch_home, switch_max;

    motor(const char* label, int in1, int in2, int quad1, int quad2, int switch_home, int switch_max, int CPR, int GR, int MM_PER_ROT)
    {
      //pins:
      this->label = label;
      this->in1 = in1;
      this->in2 = in2;
      this->quad1 = quad1;
      this->quad2 = quad2;
      this->switch_home = switch_home;
      this->switch_max = switch_max;
      //modifiers:
      this->CPR = CPR;
      this->GR = GR;
      this->MM_PER_ROT = MM_PER_ROT;
      //this->max_pos = (max_pos_mm * CPR * GR) / MM_PER_ROT;
      this->max_pos = 150000;//temporary variable

      pinMode(in1, OUTPUT);
      pinMode(in2, OUTPUT);
      pinMode(switch_home, INPUT);
      pinMode(switch_max, INPUT);

      quad = new Encoder(quad1, quad2);

      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }

    void set_goal(double goal_mm) { //from range of (ex) 0 to 1500mm
      double rotations = goal_mm / MM_PER_ROT;
      des_pos = rotations * GR * CPR;
      target_reached = false;
      /*
        int buffer_count = GR*CPR;

        if (des_pos - buffer_count < 0)
        des_pos = 0;
        if (des_pos + buffer_count> max_pos)
        des_pos = max_pos;
      */
    }

    void update_motor() {
      //update current position:
      cur_pos = quad->read();

      //limit switches:
      /*
        if (!digitalRead(switch_home)) {//if a voltage is not registered, the switch has been activated (or there is a miswiring)
        target_reached = true;
        cur_pos = 0;//recalibrate
        quad->write(0);
        des_pos = cur_pos;
        stop_motor();
        }
        if (!digitalRead(switch_max)) {
        target_reached = true;
        max_pos = cur_pos;//recalibrate
        quad->write(cur_pos);
        des_pos = cur_pos;
        stop_motor();
        }
      */
      //targeting:
      if (!target_reached) {//move towards target
        if (cur_pos + overshoot < des_pos && cur_pos + overshoot < max_pos)
          move_forward();
        else if (cur_pos - overshoot > des_pos && cur_pos - overshoot > 0)
          move_back();
        else { //near target - stop
          target_reached = true;
          stop_motor();
        }
      }
    }

    int get_goal() {
      return des_pos;
    }
    long get_pos() {
      return cur_pos;
    }
    void stop_motor() {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }
    void move_forward() {
      analogWrite(in1, 0);
      analogWrite(in2, 255);
      //digitalWrite(in1, LOW);
      //digitalWrite(in2, HIGH);
    }
    void move_back() {
      analogWrite(in2, 0);
      analogWrite(in1, 255);
      //digitalWrite(in2, LOW);
      //digitalWrite(in1, HIGH);
    }
    
};
