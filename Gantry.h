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

    void MoveGantry(const char* posX, const char* posY, const char* posZ);

    void MoveGantryX(const char* posX);
    void MoveGantryY(const char* posY);
    void MoveGantryZ(const char* posZ);

    void HomeGantry();

    void SetGantryXSpeed(const char* spX);
    void SetGantryYSpeed(const char* spY);
    void SetGantryZSpeed(const char* spZ);
};


#endif //DOEGANTRYV2_GANTRY_H
