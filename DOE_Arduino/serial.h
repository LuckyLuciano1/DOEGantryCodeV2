void(* reset_arduino) (void) = 0;

enum ARDUINO_COMMANDS {//periodic controls sent to arduino
  //one-int, immediate response commands:
  //EMPTY,//cannot interpret zero (error found via trial & error)
  TURN_ELECTROMAGNETS_OFF,
  TURN_ELECTROMAGNETS_ON,
  TURN_DRILL_OFF,
  TURN_DRILL_ON,
  TURN_FANS_OFF,
  TURN_FANS_ON,
  HOME,
  //multiple int&float, delayed response commands:
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  //exit command:
  CLOSE
};

enum ARDUINO_FEEDBACK {
  EVENT_FAILED,
  EVENT_SUCCESSFUL,
  SWITCH_HIT,
  MIN_X_HIT,
  MAX_X_HIT,
  MIN_Y_HIT,
  MAX_Y_HIT,
  MIN_Z_HIT,
  MOTORS_LOCKED
};


enum SERIAL_STATUS {
  AWAITING_NEW_COMMAND,
  GETTING_X_TARGET_VEL,
  GETTING_Y_TARGET_VEL,
  GETTING_Z_TARGET_VEL,
  GETTING_X_POS,
  GETTING_Y_POS,
  GETTING_Z_POS
};

union message {
  byte b[sizeof(float)];
  int i;
  float f;
};

void send_int(int value)
{
  message o{};
  o.i = value;
  Serial.write(o.b[0]);
  Serial.write(o.b[1]);
}
