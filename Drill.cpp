#include <iostream>
#include "Drill.h"

Drill::Drill(Arduino *arduino) {
    this->arduino = arduino;
    state = false;
}
void Drill::TurnOn() {
    std::cout<<"Turning drill on";
    state = true;
    arduino->SendInt(Arduino::TURN_DRILL_ON);
}

void Drill::TurnOff() {
    std::cout<<"Turning drill off";
    state = false;
    arduino->SendInt(Arduino::TURN_DRILL_OFF);
}

