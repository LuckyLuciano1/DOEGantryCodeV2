//
// Created by Lucas Utke on 7/14/2021.
//

#include <iostream>
#include "Gantry.h"

Gantry::Gantry(Arduino *arduino) {
    this->arduino = arduino;
}

void Gantry::MoveGantry(float posXmm, float velXmm_per_min, float posYmm, float velYmm_per_min, float posZmm, float velZmm_per_min) {
    MoveGantryXY(posXmm, velXmm_per_min, posYmm, velYmm_per_min);
    MoveGantryZ(posZmm, velZmm_per_min);
}
//TODO:test & verify
void Gantry::MoveGantryXY(float posXmm, float velXmm_per_min, float posYmm, float velYmm_per_min){
    //adjust speed proportional to triangle, such that they both complete at the same time.
    float genVel = std::max(velXmm_per_min, velYmm_per_min);
    float new_velXmm_per_min = (posXmm/posYmm)*genVel;
    float new_velYmm_per_min = (posYmm/posXmm)*genVel;

    genVel = std::max(new_velXmm_per_min, new_velYmm_per_min);
    //scale to max speed:
    if(genVel > MAX_VEL) {
        new_velXmm_per_min *= MAX_VEL / genVel;
        new_velYmm_per_min *= MAX_VEL / genVel;
    }

    MoveGantryX(posXmm, new_velXmm_per_min);
    MoveGantryY(posYmm, new_velYmm_per_min);
}
void Gantry::MoveGantryX(float posXmm, float velXmm_per_min) {
    std::cout<<"Moving Gantry X axis";
    arduino->SendInt(Arduino::MOVE_X);
    arduino->SendFloat(posXmm);
    arduino->SendFloat(velXmm_per_min);
}
void Gantry::MoveGantryY(float posYmm, float velYmm_per_min) {
    std::cout<<"Moving Gantry Y axis";
    arduino->SendInt(Arduino::MOVE_Y);
    arduino->SendFloat(posYmm);
    arduino->SendFloat(velYmm_per_min);
}
void Gantry::MoveGantryZ(float posZmm, float velZmm_per_min) {
    std::cout<<"Moving Gantry Z axis";
    arduino->SendInt(Arduino::MOVE_Z);
    arduino->SendFloat(posZmm);
    arduino->SendFloat(velZmm_per_min);
}
void Gantry::HomeGantry() {
    std::cout<<"Homing Gantry";
    arduino->SendInt(Arduino::HOME);
}