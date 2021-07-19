#include <iostream>
#include <windows.h>
#include "Arduino.h"
#include "Fans.h"
#include "Gantry.h"
#include "Electromagnets.h"
#include "Drill.h"

int main() {
    std::cout << "Initializing Objects" << std::endl;
    auto *arduino = new Arduino(R"(\\.\COM17)");
    if(arduino->IsConnected())
        std::cout<<"Arduino Connection Successful"<<std::endl;

    Drill drill(arduino);
    Electromagnets electromagnets(arduino);
    Fans fans(arduino);
    Gantry gantry(arduino);

    std::cout<<"Beginning Testing Sequence:"<<std::endl;
    gantry.MoveMotorX("50");

    /*fans.TurnOn();
    Sleep(1000);
    electromagnets.TurnOn();
    Sleep(1000);

    std::cout<<"Give Any Input To Begin Sequence: ";
    char input;
    std::cin>>input;

    gantry.MoveMotorX("50");
    Sleep(2000);
    gantry.MoveMotorX("0");
    Sleep(2000);
    gantry.MoveMotorX("25");
    Sleep(2000);
    gantry.MoveMotorX("0");
    Sleep(2000);



    //drill.TurnOn();
    //Sleep(7000);
    //drill.TurnOff();
    //Sleep(4000);

    electromagnets.TurnOff();
    fans.TurnOff();
    */
    std::cout<<"Closing Arduino";
    arduino->SendCommand(Arduino::CLOSE);
    return 0;
}
