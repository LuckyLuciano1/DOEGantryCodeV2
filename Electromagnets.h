//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_ELECTROMAGNETS_H
#define DOEGANTRYV2_ELECTROMAGNETS_H

#include "Arduino.h"

class Electromagnets {
private:
    bool state;
    Arduino *arduino;

public:
    explicit Electromagnets(Arduino *Arduino);
    void TurnOn();
    void TurnOff();

};

#endif //DOEGANTRYV2_ELECTROMAGNETS_H
