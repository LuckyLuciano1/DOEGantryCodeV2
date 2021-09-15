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

    int current_event;
    bool event_complete;
    long long pause_millisec;
    enum EVENT_LIST{
        HOMING,
        GO_TO_ZERO,
        ACTIVATE_DRILL,
        DEACTIVATE_DRILL,
        MOVE_X_START,
        MOVE_X_END
    };

    long long time;
public:
    bool exit;

    explicit EventManager(const char* portName);
    void Update();
    void StartUp();
    void ShutDown();
    static void PauseProgram();

    ~EventManager();
private:
    void InterpretFeedback(int i);
    void RunEvent(int event);
};

#endif //MAIN_CPP_EVENTMANAGER_H
