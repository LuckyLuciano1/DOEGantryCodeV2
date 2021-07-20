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
    switch (eventCount) {
        case 0:
            gantry->MoveMotorY("100");
            countdown = 300;
            break;
        case 1:
            gantry->MoveMotorX("40");
            countdown = 100;
            break;
        case 2:
            drill->TurnOn();
            countdown = 300;
            break;
        case 3:
            drill->TurnOff();
            countdown = 100;
            break;
        case 4:
            gantry->MoveMotorX("0");
            gantry->MoveMotorY("0");
            countdown = 300;
            break;
        case 5:
            //gantry->MoveMotorZ("0");
            //countdown = 400;
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



