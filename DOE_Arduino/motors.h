#include <Encoder.h>
#include <PID_v1.h>
#include "serial.h"



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

    //pololu tuning:
    int Kp = 15;
    int Ki = 0;
    int Kd = 5;
    double target_pos, target_vel, target_distance;
    double PID_setpoint = 0;
    double PID_input = 0;
    double PID_output = 255;
    double prev_cycle_time = 0;
    PID *myPID;

  public:
    bool target_reached = true;
    double current_pos;
    int buffer_count;//padding between target position and actual position to make sure the system does not overshoot. used in homing sequence.
    int motor_speed;
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
      myPID = new PID(&PID_input, &PID_output, &PID_setpoint, Kp, Ki, Kd, DIRECT);
      myPID->SetMode(AUTOMATIC);
      myPID->SetTunings(Kp, Ki, Kd);

      if (switch_min != 0)
        using_switch_min = true;
      else
        using_switch_min = false;
      if (switch_max != 0)
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

    bool update_motor() {
      //update current position:
      current_pos = encoder->read();
      
      if(!check_switches())
        return false;

      if (!target_reached) {
        //calculate delta_time:
        double current_time = micros();
        double delta_time = (current_time - prev_cycle_time) / (double) 60000000.0;
        prev_cycle_time - current_time;

        PID_setpoint += target_vel * delta_time;
        PID_input = current_pos / CPR;//in revolutions
        myPID->Compute();

        if (target_distance > 0) {
          if (target_pos - current_pos < 0) {
            brake();
            target_reached = true;
          } else
            set_PWM(FORWARD, PID_output);

        } else if (target_distance < 0) {
          if (target_pos - current_pos > 0) {
            brake();
            target_reached = true;
          } else
            set_PWM(BACK, PID_output);
        }
      }
      return true;
    }
    void set_target(int target_pos_mm, int target_vel_mm_per_min) {
      target_reached = false;
      current_pos = encoder->read();

      target_vel = (target_vel_mm_per_min) / MM_PER_ROT; //now in rpm
      target_pos = (target_pos_mm / MM_PER_ROT) * CPR;
      target_distance = target_pos - current_pos;
      PID_setpoint = abs(target_distance * 0.1f);
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
    ~motor() {
      delete myPID;
      delete encoder;
    }

  private:

    bool check_switches() {
      if (using_switch_min && digitalRead(switch_min)) {//if a voltage is not registered, the switch has been activated (or there is a miswiring)
        brake();
        if (label == "X1")
          send_int(MIN_X_HIT);
        else if (label == "Y1" || label == "Y2")
          send_int(MIN_Y_HIT);
        else if (label == "Z1")
          send_int(MIN_Z_HIT);
        return false;
      }
      if (using_switch_max && digitalRead(switch_max)) {
        brake();
        if (label == "X1")
          send_int(MAX_X_HIT);
        else if (label == "Y1" || label == "Y2")
          send_int(MAX_Y_HIT);
        return false;
      }
      return true;
    }
};
