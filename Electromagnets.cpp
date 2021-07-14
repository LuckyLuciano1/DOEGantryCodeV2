#include <iostream>
#include "Electromagnets.h"

Electromagnets::Electromagnets(Arduino *arduino) {
    this->arduino = arduino;
    state = false;
}
void Electromagnets::TurnOn() {
    std::cout<<"Turning Electromagnets on";
    state = true;
    arduino->SendCommand(Arduino::TURN_ELECTROMAGNETS_ON);
}

void Electromagnets::TurnOff() {
    std::cout<<"Turning Electromagnets off";
    state = false;
    arduino->SendCommand(Arduino::TURN_ELECTROMAGNETS_OFF);
}

