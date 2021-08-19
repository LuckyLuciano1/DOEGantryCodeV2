#include <iostream>
#include <windows.h>
#include "Arduino.h"
#include "Fans.h"
#include "Gantry.h"
#include "Electromagnets.h"
#include "Drill.h"
#include "EventManager.h"

int main() {
    EventManager eventManager(R"(\\.\COM8)");

    eventManager.StartUp();

    while(!eventManager.exit){
        eventManager.Update();
    }

    eventManager.ShutDown();
    return 0;
}
