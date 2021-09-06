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

    void MoveGantry(float posXmm, float velXmms, float posYmm, float velYmms, float posZmm, float velZmms);

    void MoveGantryX(float posXmm, float velXmms);
    void MoveGantryY(float posYmm, float velYmms);
    void MoveGantryZ(float posZmm, float velZmms);

    void HomeGantry();
};

#endif //DOEGANTRYV2_GANTRY_H
