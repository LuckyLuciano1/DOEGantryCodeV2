//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_GANTRY_H
#define DOEGANTRYV2_GANTRY_H

#include "Arduino.h"

class Gantry {
private:
    Arduino *arduino;
    const float MAX_VEL = 5.0f;
public:
    explicit Gantry(Arduino *arduino);

    void MoveGantry(float posXmm, float velXmm_per_min, float posYmm, float velYmm_per_min, float posZmm, float velZmm_per_min);
    void MoveGantryXY(float posXmm, float velXmm_per_min, float posYmm, float velYmm_per_min);
    void MoveGantryX(float posXmm, float velXmm_per_min);
    void MoveGantryY(float posYmm, float velYmm_per_min);
    void MoveGantryZ(float posZmm, float velZmm_per_min);

    void HomeGantry();
};

#endif //DOEGANTRYV2_GANTRY_H
