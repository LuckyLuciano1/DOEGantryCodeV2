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
    fans.TurnOn();
    Sleep(1000);
    electromagnets.TurnOn();
    Sleep(1000);

    std::cout<<"Give Any Input To Begin Drill Sequence: ";
    char input;
    std::cin>>input;

    drill.TurnOn();
    Sleep(5000);
    drill.TurnOff();
    Sleep(5000);
    electromagnets.TurnOff();
    fans.TurnOff();

    return 0;
}
