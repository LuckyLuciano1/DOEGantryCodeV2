//
// Created by Lucas Utke on 7/14/2021.
//

#include <iostream>
#include "Gantry.h"

Gantry::Gantry(Arduino *arduino) {
    this->arduino = arduino;
}

void Gantry::MoveMotor(const char* posXmm, const char* posYmm, const char* posZmm) {
    std::cout<<"Moving Motor";
    arduino->SendLongCommand(Arduino::MOVE_X, posXmm);
    arduino->SendLongCommand(Arduino::MOVE_Y, posYmm);
    arduino->SendLongCommand(Arduino::MOVE_Z, posZmm);
}
void Gantry::MoveMotorX(const char* posXmm) {
    std::cout<<"Moving Motor";
    arduino->SendLongCommand(Arduino::MOVE_X, posXmm);
}
void Gantry::MoveMotorY(const char* posYmm) {
    std::cout<<"Moving Motor";
    arduino->SendLongCommand(Arduino::MOVE_Y, posYmm);
}
void Gantry::MoveMotorZ(const char* posZmm) {
    std::cout<<"Moving Motor";
    arduino->SendLongCommand(Arduino::MOVE_Z, posZmm);
}
