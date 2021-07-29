//
// Created by Lucas Utke on 7/19/2021.
//

#include <iostream>
#include "EventManager.h"

EventManager::EventManager(const char *portName) {
    exit = false;
    countdown = 0;
    eventCount = 0;

    std::cout << "Initializing Objects" << std::endl;
    arduino = new Arduino(R"(\\.\COM17)");

    gantry = new Gantry(arduino);
    drill = new Drill(arduino);
    electromagnets = new Electromagnets(arduino);
    fans = new Fans(arduino);
}

void EventManager::RunNextCommand() {
    enum EVENT_ORDER{
        HOMING,
        TOGGLE_DRILL,
        MOVE_X_START,
        MOVE_X_END
    };

    switch (eventCount) {
        case HOMING:
            std::cout<<"------------------------Beginning Homing Procedure------------------------"<<std::endl;
            ManualStall();
            gantry->HomeGantry();
            ManualStall();
            countdown = 30;
            break;
        case TOGGLE_DRILL:
            //gantry->MoveGantryX("250");
            //ManualStall();
            /*std::cout<<"------------------------     Activating Drill     ------------------------"<<std::endl;
            ManualStall();
            drill->TurnOn();
            ManualStall();
            drill->TurnOff();
            countdown = 500;*/
            break;
        case MOVE_X_START:
            /*gantry->SetGantryXSpeed("100");
            gantry->MoveGantryX("20");
            countdown = 100;*/
            break;
        case MOVE_X_END:
            /*gantry->SetGantryXSpeed("100");
            gantry->MoveGantryX("0");
            countdown = 100*/
            break;
        default://if all commands have been run
            std::cout<<"All commands have been processed, exiting event manager"<<std::endl;
            exit = true;
        break;

    }
    eventCount++;
}

void EventManager::StartUp() {
    std::cout<<"------------------------Beginning Start Up Procedure------------------------"<<std::endl;
    Sleep(1000);
    fans->TurnOn();
    electromagnets->TurnOn();
    Sleep(1000);
}

void EventManager::Update() {
    if (countdown <= 0)//if the allotted time has passed from the last command,
        RunNextCommand();
    else
        countdown--;

    //receive error warnings from arduino here (or other repeated data)

    //~100fps - need a more robust timer
    Sleep(10);
}

void EventManager::ShutDown() {
    std::cout<<"------------------------Beginning Shut Down Procedure------------------------"<<std::endl;
    fans->TurnOff();
    drill->TurnOff();//redundancy
    electromagnets->TurnOff();
    Sleep(1000);
}

EventManager::~EventManager() {
    delete drill;
    delete electromagnets;
    delete gantry;
    delete fans;

    std::cout << "Closing Arduino";
    arduino->SendCommand(Arduino::CLOSE);
    delete arduino;
}

void EventManager::ManualStall() {
    std::cout<<"Manual Stall Initiated, Give Input to Continue: ";
    char input;
    std::cin>>input;
}



