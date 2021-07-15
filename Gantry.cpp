//
// Created by Lucas Utke on 7/14/2021.
//

#include <iostream>
#include "Gantry.h"

Gantry::Gantry(Arduino *arduino) {
    this->arduino = arduino;
}

void Gantry::MoveMotor() {
    std::cout<<"Moving Motor";
    arduino->SendCommand(Arduino::MOVE_MOTOR);
}
