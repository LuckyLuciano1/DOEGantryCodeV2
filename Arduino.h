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
        TURN_ELECTROMAGNETS_OFF,
        TURN_ELECTROMAGNETS_ON,
        TURN_DRILL_OFF,
        TURN_DRILL_ON,
        TURN_FANS_OFF,
        TURN_FANS_ON,
        MOVE_X,
        MOVE_Y,
        MOVE_Z,
        CLOSE
    };

    explicit Arduino(const char *portName);
    ~Arduino();

    void SendCommand(int command);
    void SendLongCommand(int command, const char * data);

    bool ReadData(char *buffer, unsigned int nbChar);
    bool WriteData(const char *buffer, unsigned int nbChar);

    bool IsConnected() const{ return connected; }
};


#endif //DOEGANTRYV2_ARDUINO_H
