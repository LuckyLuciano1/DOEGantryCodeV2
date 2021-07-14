#include <iostream>
#include "Fans.h"

Fans::Fans(Arduino *arduino) {
    this->arduino = arduino;
    state = false;
}
void Fans::TurnOn() {
    std::cout<<"Turning Fans on";
    state = true;
    arduino->SendCommand(Arduino::TURN_FANS_ON);
}

void Fans::TurnOff() {
    std::cout<<"Turning Fans off";
    state = false;
    arduino->SendCommand(Arduino::TURN_FANS_OFF);
}

