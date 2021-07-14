//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_DRILL_H
#define DOEGANTRYV2_DRILL_H


#include "Arduino.h"

class Drill {
    private:
    bool state;
    Arduino *arduino;

public:
    explicit Drill(Arduino *Arduino);
    void TurnOn();
    void TurnOff();

};


#endif //DOEGANTRYV2_DRILL_H
