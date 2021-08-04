#include <Encoder.h>

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
    int buffer_count = 0;
    int motor_speed = 255;
    long cur_pos = 0;
    long des_pos = 0;
    long max_pos = 0;
    Encoder *quad;
    int switch_min, switch_max;
    bool using_switch_min, using_switch_max;

    motor(const char* label, int in1, int in2, int quad1, int quad2, int switch_min, int switch_max, int CPR, int GR, int MM_PER_ROT, bool using_min_switch, bool using_max_switch)
    {
      //pins:
      this->label = label;
      this->in1 = in1;
      this->in2 = in2;
      this->quad1 = quad1;
      this->quad2 = quad2;
      this->switch_min = switch_min;
      this->switch_max = switch_max;
      this->using_switch_min = using_switch_min;
      this->using_switch_max = using_switch_max;
      //modifiers:
      this->CPR = CPR;
      this->GR = GR;
      this->MM_PER_ROT = MM_PER_ROT;
      this->max_pos = 150 * CPR * GR; //temporary maximum - calibrated during homing

      motor_speed = 255;
      buffer_count = GR * CPR * 0.5;
      pinMode(in1, OUTPUT);
      pinMode(in2, OUTPUT);
      
      if (using_switch_min)
        pinMode(switch_min, INPUT_PULLUP);
      if (using_switch_max)
        pinMode(switch_max, INPUT_PULLUP);

      quad = new Encoder(quad1, quad2);

      //start in off position:
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }

    void set_goal(double goal_mm) { //from range of (ex) 0 to 1500mm
      double rotations = goal_mm / MM_PER_ROT;
      des_pos = rotations * GR * CPR;
      target_reached = false;

      //add buffer to avoid bumping into pins at edges
      if (des_pos < buffer_count)
        des_pos = buffer_count;
      if (des_pos > max_pos - buffer_count)
        des_pos = max_pos - buffer_count;
    }

    void update_motor() {
      //update current position:
      cur_pos = quad->read();

      //limit switches:
/*
      if (using_switch_min && digitalRead(switch_min)) {//if a voltage is not registered, the switch has been activated (or there is a miswiring)
        target_reached = true;
        cur_pos = 0;//recalibrate
        quad->write(0);
        des_pos = cur_pos;
        stop_motor();
      }
      if (using_switch_max && digitalRead(switch_max)) {
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
    void move_back() {// axis_PWM : int dir, float motor_PWM - dir +/-, motor_speed 0-255
      analogWrite(in2, 0);
      analogWrite(in1, 255);
      //digitalWrite(in2, LOW);
      //digitalWrite(in1, HIGH);
    }
    //Z transform for PD control for position
};
