//
// Created by Lucas Utke on 7/14/2021.
//

#include <iostream>
#include "Gantry.h"

Gantry::Gantry(Arduino *arduino) {
    this->arduino = arduino;
}

void Gantry::MoveGantry(float posXmm, float velXmms, float posYmm, float velYmms, float posZmm, float velZmms) {
    MoveGantryXY(posXmm, velXmms, posYmm, velYmms);
    MoveGantryZ(posZmm, velZmms);
}
//TODO:test & verify
void Gantry::MoveGantryXY(float posXmm, float velXmms, float posYmm, float velYmms){
    //adjust speed proportional to triangle, such that they both complete at the same time.
    float genVel = std::max(velXmms, velYmms);
    float new_velXmms = (posXmm/posYmm)*genVel;
    float new_velYmms = (posYmm/posXmm)*genVel;

    genVel = std::max(new_velXmms, new_velYmms);
    //scale to max speed:
    if(genVel > MAX_VEL) {
        new_velXmms *= MAX_VEL / genVel;
        new_velYmms *= MAX_VEL / genVel;
    }

    MoveGantryX(posXmm, new_velXmms);
    MoveGantryY(posYmm, new_velYmms);
}
void Gantry::MoveGantryX(float posXmm, float velXmms) {
    std::cout<<"Moving Gantry X axis";
    arduino->SendInt(Arduino::MOVE_X);
    arduino->SendFloat(posXmm);
    arduino->SendFloat(velXmms);
}
void Gantry::MoveGantryY(float posYmm, float velYmms) {
    std::cout<<"Moving Gantry Y axis";
    arduino->SendInt(Arduino::MOVE_Y);
    arduino->SendFloat(posYmm);
    arduino->SendFloat(velYmms);
}
void Gantry::MoveGantryZ(float posZmm, float velZmms) {
    std::cout<<"Moving Gantry Z axis";
    arduino->SendInt(Arduino::MOVE_Z);
    arduino->SendFloat(posZmm);
    arduino->SendFloat(velZmms);
}
void Gantry::HomeGantry() {
    std::cout<<"Homing Gantry";
    arduino->SendInt(Arduino::HOME);
}