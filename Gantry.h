//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_GANTRY_H
#define DOEGANTRYV2_GANTRY_H


#include "Arduino.h"

class Gantry {
private:
    Arduino *arduino;
public:
    explicit Gantry(Arduino *arduino);

    void MoveMotor(const char* posX, const char* posY, const char* posZ);

    void MoveMotorX(const char* posX);

    void MoveMotorY(const char* posY);

    void MoveMotorZ(const char* posZ);
};


#endif //DOEGANTRYV2_GANTRY_H
