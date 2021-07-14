//
// Created by Lucas Utke on 7/14/2021.
//

#ifndef DOEGANTRYV2_GANTRY_H
#define DOEGANTRYV2_GANTRY_H


#include "Arduino.h"

class Gantry {
private:
    Arduino *arduino;
public:
    explicit Gantry(Arduino *arduino);
};


#endif //DOEGANTRYV2_GANTRY_H
