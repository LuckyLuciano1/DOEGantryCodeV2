//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_ARDUINO_H
#define DOEGANTRYV2_ARDUINO_H

#include <windows.h>
#include <cstdio>
#include <cstdlib>

class Arduino {
private:
    //Serial Communication variables:
    HANDLE hSerial;//serial comm handler
    bool connected;
    COMSTAT status;//connection information
    DWORD errors;//track last error

public:
    enum ARDUINO_COMMANDS {
        //one-int, immediate response commands:
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
        MAX_Z_HIT,
        MOTORS_LOCKED,
        EXIT_AND_RESET
    };
    explicit Arduino(const char *portName);
    ~Arduino();

    void SendInt(int data);
    void SendFloat(float data);
    void SendChar(char data);

    bool ReadData(byte *buffer, unsigned int nbChar);
    bool WriteData(const char *buffer, unsigned int nbChar);

    bool IsConnected() const{ return connected; }
};


#endif //DOEGANTRYV2_ARDUINO_H
