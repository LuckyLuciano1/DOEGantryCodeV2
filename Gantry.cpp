//
// Created by Lucas Utke on 7/14/2021.
//

#include <iostream>
#include "Gantry.h"

Gantry::Gantry(Arduino *arduino) {
    this->arduino = arduino;
}

void Gantry::MoveGantry(const char* posXmm, const char* posYmm, const char* posZmm) {
    std::cout<<"Moving Gantry";
    arduino->SendLongCommand(Arduino::MOVE_X, posXmm);
    arduino->SendLongCommand(Arduino::MOVE_Y, posYmm);
    arduino->SendLongCommand(Arduino::MOVE_Z, posZmm);
}
void Gantry::MoveGantryX(const char* posXmm) {
    std::cout<<"Moving Gantry";
    arduino->SendLongCommand(Arduino::MOVE_X, posXmm);
}
void Gantry::MoveGantryY(const char* posYmm) {
    std::cout<<"Moving Gantry";
    arduino->SendLongCommand(Arduino::MOVE_Y, posYmm);
}
void Gantry::MoveGantryZ(const char* posZmm) {
    std::cout<<"Moving Gantry";
    arduino->SendLongCommand(Arduino::MOVE_Z, posZmm);
}
void Gantry::HomeGantry() {
    std::cout<<"Homing Gantry";
    arduino->SendCommand(Arduino::HOME);
}
