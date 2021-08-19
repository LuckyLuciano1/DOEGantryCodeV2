#include <Encoder.h>
#include <PID_v1.h>

//type-specific config variables:
int pololu_CPR = 64;
int pololu_GR = 30;
int pololu_MM_PER_ROT = 5;
//int pololu_overshoot = 384 * 1.7;
int maxon_CPR = 500;
int maxon_GR = 308;
int maxon_MM_PER_ROT = 93;
//int maxon_overshoot = 500;//unconfigured

enum MOTOR_TYPE {
  POLOLU37D,
  MAXON353301
};
enum DIRECTION {
  BACK,
  FORWARD
};
int GANTRY_FAST_TRAVEL = 5;//standard input velocity for motors

class motor {
  private:
    double GR;//gear ratio
    double CPR;//encoder counts per rotation
    double MM_PER_ROT;//linear travel per rotation (mm)

    int PWM1, PWM2;//, quad1, quad2;//motor pins
    const char* label;

  public:
    int buffer_count;//padding between target position and actual position to make sure the system does not overshoot. used in homing sequence.
    int motor_speed;

    long cur_pos = 0;
    long max_pos = 0;
    Encoder *encoder;

    int switch_min, switch_max;//switch pins
    bool using_switch_min, using_switch_max;

    motor(const char* label, int PWM1, int PWM2, int quad1, int quad2, int switch_min, int switch_max, int motor_type)
    {
      this->label = label;

      //pins:
      this->PWM1 = PWM1;
      this->PWM2 = PWM2;
      this->switch_min = switch_min;
      this->switch_max = switch_max;
      encoder = new Encoder(quad1, quad2);

      if (switch_min != NULL)
        using_switch_min = true;
      else
        using_switch_min = false;
      if (switch_max != NULL)
        using_switch_max = true;
      else
        using_switch_max = false;

      //motor modifiers:
      if (motor_type == POLOLU37D) {
        CPR = pololu_CPR;
        GR = pololu_GR;
        MM_PER_ROT = pololu_MM_PER_ROT;
      }
      else if (motor_type == MAXON353301) {
        CPR = maxon_CPR;
        GR = maxon_GR;
        MM_PER_ROT = maxon_MM_PER_ROT;
      }

      max_pos = 150 * CPR * GR; //temporary maximum - calibrated during homing
      motor_speed = 255;
      buffer_count = GR * CPR * 0.5;

      pinMode(PWM1, OUTPUT);
      pinMode(PWM2, OUTPUT);

      if (using_switch_min)
        pinMode(switch_min, INPUT_PULLUP);
      if (using_switch_max)
        pinMode(switch_max, INPUT_PULLUP);

      //start motor in off position:
      digitalWrite(PWM1, LOW);
      digitalWrite(PWM2, LOW);
    }

    void update_motor() {
      //update current position:
      cur_pos = encoder->read();

      //limit switches:
      /*
            if (using_switch_min && digitalRead(switch_min)) {//if a voltage is not registered, the switch has been activated (or there is a miswiring)
              target_reached = true;
              cur_pos = 0;//recalibrate
              encoder->write(0);
              des_pos = cur_pos;
              stop_motor();
            }
            if (using_switch_max && digitalRead(switch_max)) {
              target_reached = true;
              max_pos = cur_pos;//recalibrate
              encoder->write(cur_pos);
              des_pos = cur_pos;
              stop_motor();
            }
      */
      //targeting:
      /*
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
      */
    }

    /*
        void set_goal(double goal_mm) {//from range of (ex) 0 to 1500mm
          double rotations = goal_mm / MM_PER_ROT;
          des_pos = rotations * GR * CPR;
          target_reached = false;

          //add buffer to avoid bumping into pins at edges
          if (des_pos < buffer_count)
            des_pos = buffer_count;
          if (des_pos > max_pos - buffer_count)
            des_pos = max_pos - buffer_count;
        }
    */
    void set_target(int target_pos, int target_vel) {

    }
    void set_PWM(bool dir, uint8_t pwm) {
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

    void brake() {
      analogWrite(PWM1, LOW);
      analogWrite(PWM2, LOW);
    }
};
