#include <Encoder.h>
#include <PID_v1.h>
#include "serial.h"

//type-specific config variables:
int pololu_CPR = 64;
int pololu_GR = 30;//300*0.85;
int pololu_MM_PER_ROT = 5;
int maxon_CPR = 500;
int maxon_GR = 308;//*4;
int maxon_MM_PER_ROT = 93;

enum MOTOR_TYPE {
  POLOLU37D,
  MAXON353301
};
enum DIRECTION {
  BACK,
  FORWARD
};
int GANTRY_FAST_TRAVEL = 5;//standard input velocity for motors

class limit_switch {
  private:
    int active_for, pin;
    const int buffer = 2;
  public:
    limit_switch(int given_pin) {
      active_for = 0;
      pin = given_pin;
      pinMode(given_pin, INPUT_PULLUP);
    }
    void update() {
      if (digitalRead(pin))
        active_for++;
      else
        active_for = 0;
    }
    bool read() {
      return (active_for >= buffer);
    }
};

class motor {
  private:
    double GR;//gear ratio
    double CPR;//encoder counts per rotation
    double MM_PER_ROT;//linear travel per rotation (mm)

    int PWM1, PWM2;//, quad1, quad2;//motor pins
    const char* label;

    //pololu tuning:
    int Kp = 105;
    int Ki = 6;
    int Kd = 2;
    double target_pos, target_vel, target_distance;
    double PID_setpoint, PID_input, PID_output;
    double prev_cycle_time = 0;
    PID *myPID;

  public:
    bool target_reached = true;//check if issue here
    double current_pos = 0;
    int buffer_count;//padding between target position and actual position to make sure the system does not overshoot. used in homing sequence.
    long max_pos = 0;
    Encoder *encoder;
    limit_switch *switch_min;
    limit_switch *switch_max;
    bool using_switch_min, using_switch_max;

    motor(const char* label, int PWM1, int PWM2, int quad1, int quad2, limit_switch *switch_min, limit_switch *switch_max, int motor_type)
    {
      this->label = label;

      //pins:
      this->PWM1 = PWM1;
      this->PWM2 = PWM2;
      this->switch_min = switch_min;
      this->switch_max = switch_max;
      
      encoder = new Encoder(quad1, quad2);
      encoder->write(1);//may not be necessary for it to be 1 rather than 0
      
      myPID = new PID(&PID_input, &PID_output, &PID_setpoint, Kp, Ki, Kd, DIRECT);
      myPID->SetMode(AUTOMATIC);
      myPID->SetTunings(Kp, Ki, Kd);

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
      buffer_count = GR * CPR * 0.5;

      pinMode(PWM1, OUTPUT);
      pinMode(PWM2, OUTPUT);

      //start motor in off position:
      digitalWrite(PWM1, LOW);
      digitalWrite(PWM2, LOW);
    }

    bool update() {
      //update current position:
      current_pos = encoder->read();

      if (!limit_check())
        return false;
        
      if (!target_reached) {
        //calculate delta_time:
        double current_time = micros();
        double delta_time = (current_time - prev_cycle_time) / (double) 60000000.0;
        prev_cycle_time = current_time;

        PID_setpoint += target_vel * delta_time;
        PID_input = current_pos / (CPR * GR); //in revolutions
        myPID->Compute();

        if (target_distance > 0) {
          if (target_pos / CPR - PID_input < 0) {
            brake();
            target_reached = true;
          } else
            set_PWM(FORWARD, PID_output);

        } else if (target_distance < 0) {
          if (target_pos / CPR - PID_input > 0) {
            brake();
            target_reached = true;
          } else
            set_PWM(BACK, PID_output);
        }
      }
      return true;
    }
    void set_target(int target_pos_mm, int target_vel_rpm) {
      target_reached = false;
      current_pos = encoder->read();

      //target_vel = (target_vel_mm_per_min) / MM_PER_ROT; //now in rpm
      target_vel = 330.0 * (target_vel_rpm / (225.0 * 5.0));
      target_pos = (target_pos_mm / MM_PER_ROT) * CPR;
      target_distance = target_pos - current_pos;
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
      delete switch_min;
      delete switch_max;
    }

  private:

    bool limit_check() {
      if (using_switch_min && switch_min->read()) {//if a voltage is not registered, the limit_switchhas been activated (or there is a miswiring)
        brake();
        if (label == "X1")
          send_int(MIN_X_HIT);
        else if (label == "Y1" || label == "Y2")
          send_int(MIN_Y_HIT);    
        return false;
      }
      if (using_switch_max && switch_max->read()) {
        brake();
        if (label == "X1")
          send_int(MAX_X_HIT);
        else if (label == "Y1" || label == "Y2")
          send_int(MAX_Y_HIT);
        else if (label == "Z1")
          send_int(MAX_Z_HIT);
        return false;
      }
      return true;
    }
};
