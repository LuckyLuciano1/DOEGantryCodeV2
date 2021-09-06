//
// Created by Lucas Utke on 7/14/2021.
//

#include <iostream>
#include "Gantry.h"

Gantry::Gantry(Arduino *arduino) {
    this->arduino = arduino;
}

void Gantry::MoveGantry(float posXmm, float velXmms, float posYmm, float velYmms, float posZmm, float velZmms) {
    MoveGantryX(posXmm, velXmms);
    MoveGantryY(posYmm, velYmms);
    MoveGantryZ(posZmm, velZmms);
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