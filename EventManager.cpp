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
    arduino = new Arduino(portName);

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
            //gantry->MoveGantryX(-100, 5);//currently untested
            gantry->HomeGantry();
            ManualStall();
            countdown = 10;
            break;
        case TOGGLE_DRILL:
            //ManualStall();
            /*std::cout<<"------------------------     Activating Drill     ------------------------"<<std::endl;
            ManualStall();
            drill->TurnOn();
            ManualStall();
            drill->TurnOff();
            countdown = 500;*/
            break;
        case MOVE_X_START:

            break;
        case MOVE_X_END:

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

    //receive feedback from arduino here (only integers, which are then reinterpreted into enum ARDUINO_FEEDBACK)
    byte data[2];
    if(arduino->ReadData(data, 2)){
        union message {
            byte b[2];
            int i;
        };
        message m{};
        for(int x = 0; x < 2; x++){
            m.b[x] = data[x];
        }

        switch (m.i) {
            case Arduino::HOMING_FAILED:
                std::cout<<"Message Received - value "<<m.i<<" - Homing Failed, Arduino Reset"<<std::endl;
                exit = true;
                break;
            case Arduino::HOMING_SUCCESSFUL:
                std::cout<<"Message Received - value "<<m.i<<" - Homing Succeeded"<<std::endl;
                break;
            case Arduino::SWITCH_HIT:
                std::cout<<"Message Received - value "<<m.i<<" - Switch was Hit"<<std::endl;
                break;
            case Arduino::MIN_X_HIT:
                std::cout<<"Message Received - value "<<m.i<<" - MIN X Switch was Hit"<<std::endl;
                break;
            case Arduino::MAX_X_HIT:
                std::cout<<"Message Received - value "<<m.i<<" - MAX X Switch was Hit"<<std::endl;
                break;
            case Arduino::MIN_Y_HIT:
                std::cout<<"Message Received - value "<<m.i<<" - MIN Y Switch was Hit"<<std::endl;
                break;
            case Arduino::MAX_Y_HIT:
                std::cout<<"Message Received - value "<<m.i<<" - MAX Y Switch was Hit"<<std::endl;
                break;
            case Arduino::MIN_Z_HIT:
                std::cout<<"Message Received - value "<<m.i<<" - MIN Z Switch was Hit"<<std::endl;
                break;
            default:
                std::cout<<"Message Received - value "<<m.i<<" - unrecognized code, check enum ARDUINO_ERRORS"<<std::endl;
                break;
        }
    }

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
    arduino->SendInt(Arduino::CLOSE);
    delete arduino;
}

void EventManager::ManualStall() {
    std::cout<<"Manual Stall Initiated, Give Input to Continue: ";
    char input;
    std::cin>>input;
}



