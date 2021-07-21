//
// Created by Lucas Utke on 7/19/2021.
//

#ifndef MAIN_CPP_EVENTMANAGER_H
#define MAIN_CPP_EVENTMANAGER_H


#include "Electromagnets.h"
#include "Fans.h"
#include "Gantry.h"
#include "Drill.h"
#include "Arduino.h"

class EventManager {
private:
    Arduino* arduino;
    Drill* drill;
    Electromagnets* electromagnets;
    Fans* fans;
    Gantry* gantry;

    int countdown;
    int eventCount;
public:
    bool exit;

    explicit EventManager(const char* portName);
    void Update();
    void StartUp();
    void ShutDown();
    void RunNextCommand();
    void ManualStall();
    ~EventManager();
};


#endif //MAIN_CPP_EVENTMANAGER_H
