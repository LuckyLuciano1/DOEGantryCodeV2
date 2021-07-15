//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_ARDUINO_H
#define DOEGANTRYV2_ARDUINO_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

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
        MOVE_MOTOR,
        CLOSE
    };

    explicit Arduino(const char *portName);
    ~Arduino();
    void SendCommand(int command);
    void SendLongCommand();//currently unimplemented

    //Read data in a buffer, if nbChar is greater than the
    //maximum number of bytes available, it will return only the
    //bytes available. The function return -1 when nothing could
    //be read, the number of bytes actually read.
    int ReadData(char *buffer, unsigned int nbChar);
    //Writes data from a buffer through the Serial connection
    //return true on success.
    bool WriteData(const char *buffer, unsigned int nbChar);
    //Check if we are actually connected
    bool IsConnected(){ return this->connected; }
};


#endif //DOEGANTRYV2_ARDUINO_H
