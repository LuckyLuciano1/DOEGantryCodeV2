//
// Created by Lucas Utke on 7/19/2021.
//

#include <chrono>
#include <iostream>
#include "EventManager.h"

EventManager::EventManager(const char *portName) {
    exit = false;
    event_complete = true;
    current_event = 0;
    pause_millisec = 0;
    time = 0;

    std::cout << "Initializing Objects" << std::endl;
    arduino = new Arduino(portName);

    gantry = new Gantry(arduino);
    drill = new Drill(arduino);
    electromagnets = new Electromagnets(arduino);
    fans = new Fans(arduino);
}

void EventManager::StartUp() {
    std::cout<<"------------------------Beginning Start Up Procedure------------------------"<<std::endl;
    Sleep(1000);
    fans->TurnOn();
    electromagnets->TurnOn();
    Sleep(1000);
}

void EventManager::Update() {
    int event_order[]{
        HOMING,
        GO_TO_START,
        ACTIVATE_DRILL,
        PLUNGE,
        GO_TO_END,
        RETRACT,
        DEACTIVATE_DRILL,
        GO_TO_ZERO
    };
    //update clock:
    long long prev_time = time;

    long long new_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    long long elapsed_time = new_time - prev_time;

    if(event_complete && pause_millisec > 0)
        pause_millisec-= elapsed_time;

    time = new_time;

    //run new event or exit on all events run:
    if(event_complete && pause_millisec <= 0) {
        int event_num = sizeof(event_order)/sizeof(int);
        if (current_event > event_num)//all events run
            exit = true;
        else
            RunEvent(event_order[current_event]);

        current_event++;
        event_complete = false;

    } else {
        //hear from arduino for event completion:
        //(only integers, which are then reinterpreted into enum ARDUINO_FEEDBACK)
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
            //print feedback:
            InterpretFeedback(m.i);
            //determine event success/failure:
            if(m.i == Arduino::EVENT_SUCCESSFUL)
                event_complete = true;
        }
    }
}

void EventManager::RunEvent(int event) {
    switch (event) {
        case HOMING:
            std::cout << "------------------------Beginning Homing Procedure------------------------" << std::endl;
            PauseProgram();
            gantry->HomeGantry();
            pause_millisec = 250;
            break;
        case GO_TO_ZERO:
            gantry->MoveGantryX(1.0f, 1650.0f);
            gantry->MoveGantryY(1.0f, 1650.0f);
            pause_millisec = 500;
            break;
        case GO_TO_START:
            gantry->MoveGantryX(14.0f, 1650.0f);
            pause_millisec = 500;
            break;
        case GO_TO_END:
            gantry->MoveGantryY(20.0f, 10.0f);
            pause_millisec = 500;
            break;
        case PLUNGE:
            gantry->MoveGantryZ(-1.0f, 20.0f);
            pause_millisec = 500;
            break;
        case RETRACT:
            gantry->MoveGantryZ(30.0f, 20.0f);
            pause_millisec = 500;
            break;
        case ACTIVATE_DRILL:
            std::cout<<"------------------------     Activating Drill     ------------------------"<<std::endl;
            PauseProgram();
            drill->TurnOn();
            pause_millisec = 5000;
            break;
        case DEACTIVATE_DRILL:
            drill->TurnOff();
            pause_millisec = 5000;
            break;
        default://if all commands have been run
            std::cout << "Unknown event given or event list complete, exiting event manager" << std::endl;
            exit = true;
            break;
    }
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

void EventManager::PauseProgram() {
    std::cout<<"Manual Stall Initiated, Give Input to Continue: ";
    char input;
    std::cin>>input;
}

void EventManager::InterpretFeedback(int i) {
    switch (i) {
        case Arduino::EVENT_FAILED:
            std::cout<<"Message Received - value "<<i<<" - Event Failed, Arduino Reset"<<std::endl;
            exit = true;
            break;
        case Arduino::EVENT_SUCCESSFUL:
            std::cout<<"Message Received - value "<<i<<" - Event Succeeded"<<std::endl;
            break;
        case Arduino::SWITCH_HIT:
            std::cout<<"Message Received - value "<<i<<" - Switch was Hit"<<std::endl;
            break;
        case Arduino::MIN_X_HIT:
            std::cout<<"Message Received - value "<<i<<" - MIN X Switch was Hit"<<std::endl;
            break;
        case Arduino::MAX_X_HIT:
            std::cout<<"Message Received - value "<<i<<" - MAX X Switch was Hit"<<std::endl;
            break;
        case Arduino::MIN_Y_HIT:
            std::cout<<"Message Received - value "<<i<<" - MIN Y Switch was Hit"<<std::endl;
            break;
        case Arduino::MAX_Y_HIT:
            std::cout<<"Message Received - value "<<i<<" - MAX Y Switch was Hit"<<std::endl;
            break;
        case Arduino::MIN_Z_HIT:
            std::cout<<"Message Received - value "<<i<<" - MIN Z Switch was Hit"<<std::endl;
            break;
        case Arduino::MAX_Z_HIT:
            std::cout<<"Message Received - value "<<i<<" - MAX Z Switch was Hit"<<std::endl;
            break;
        case Arduino::MOTORS_LOCKED:
            std::cout<<"Message Received - value "<<i<<" - MOTORS ARE LOCKED DUE TO SWITCH BEING HIT - CLOSING"<<std::endl;
            exit = true;
            break;
        case Arduino::EXIT_AND_RESET:
            std::cout<<"Message Received - value "<<i<<" - ARDUINO IS NOW EXITING AND RESETTING"<<std::endl;
            exit = true;
            break;
        default:
            std::cout<<"Message Received - value "<<i<<" - unrecognized code, check enum ARDUINO_FEEDBACK"<<std::endl;
            break;
    }
}

