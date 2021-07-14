//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_FANS_H
#define DOEGANTRYV2_FANS_H

#include "Arduino.h"

class Fans {
private:
    bool state;
    Arduino *arduino;

public:
    explicit Fans(Arduino *Arduino);
    void TurnOn();
    void TurnOff();

};

#endif //DOEGANTRYV2_FANS_H
