#include <iostream>
#include "Arduino.h"

Arduino::Arduino(const char *portName) {
    this->connected = false;
    //attempt to connect:
    this->hSerial = CreateFile(portName,
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    //check if connection was successful:
    if (this->hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
        else
            printf("ERROR!!!");
    }
    else {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(hSerial, &dcbSerialParams))
            printf("failed to get current serial parameters!");
        else {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = CBR_9600;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if (!SetCommState(hSerial, &dcbSerialParams))
                printf("ALERT: Could not set Serial Port parameters - ");

            //Set timeout parameters
            COMMTIMEOUTS timeouts = {0};
            timeouts.ReadIntervalTimeout = 50;
            timeouts.ReadTotalTimeoutConstant = 50;
            timeouts.ReadTotalTimeoutMultiplier = 10;
            timeouts.WriteTotalTimeoutConstant = 50;
            timeouts.WriteTotalTimeoutMultiplier = 10;
            if (!SetCommTimeouts(hSerial, &timeouts))
                printf("failed to set timeouts!");

            //If everything went fine we're connected
            connected = true;
            //Flush any remaining characters in the buffers
            PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
            //Wait 2s as the arduino board will be resetting
            printf("Resetting Board... ");
            Sleep(2000);

            if(IsConnected())
                std::cout<<"Arduino Connection Successful"<<std::endl;
        }
    }
}
void Arduino::SendInt(int data){
    union message{
        int value;
        byte b[sizeof(int)];
    };
    union message m{};
    m.value = data;

    if (!WriteData(reinterpret_cast<const char *>(&m.b[0]), sizeof(int)))
        std::cout << ", data failed to send." << std::endl;
    else
        std::cout << ", data successfully sent." << std::endl;
}
void Arduino::SendFloat(float data){
    union message{
        float value;
        byte b[sizeof(float)];
    };
    union message m{};
    m.value = data;

    std::cout<<"- sending value "<<m.value;
    if (!WriteData(reinterpret_cast<const char *>(&m.b[0]), sizeof(float)))
        std::cout << ", data failed to send." << std::endl;
    else
        std::cout << ", data successfully sent." << std::endl;
}
void Arduino::SendChar(char data){
    std::cout<<"- sending value "<<data;
    if (!WriteData(&data, sizeof(char)))
        std::cout << ", data failed to send." << std::endl;
    else
        std::cout << ", data successfully sent." << std::endl;
}
/*
void Arduino::SendCommand(int command) {
    command += 65;
    char com = command;
    const char *c_command = &com;
    std::cout << ", sending command: " << (char) command;

    if (!WriteData(c_command, 5))
        std::cout << ", data failed to send." << std::endl;
    else
        std::cout << ", data successfully sent." << std::endl;
}

void Arduino::SendLongCommand(int command, const char *data) {
    command += 65;
    std::string s_command(1, (char)command);

    char buffer[256];
    strncpy(buffer, s_command.c_str(), sizeof(buffer));
    strncat(buffer, data, sizeof(buffer));
    strncat(buffer, "~", sizeof(buffer));//give ending flag

    std::cout << ", sending command: " << buffer;

    if (!WriteData(buffer, sizeof(buffer)))
        std::cout << ", data failed to send, ";
    else
        std::cout << ", data successfully sent, ";

    memset(buffer, 0, sizeof(buffer));//clear buffer

    if (!ReadData(buffer, 256))
        std::cout << "received no data back." << std::endl;
    else
        std::cout << "received back: " << buffer<<std::endl;
}*/

bool Arduino::ReadData(byte *buffer, unsigned int nbChar) {
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(hSerial, &errors, &status);
    //Check if there is something to read
    if (status.cbInQue > 0) {
        if(nbChar > status.cbInQue)
            return false;
        if (ReadFile(hSerial, buffer, nbChar, &bytesRead, nullptr)) {
            return true;
        }
    }
    return false;
}

bool Arduino::WriteData(const char *buffer, unsigned int nbChar) {
    DWORD bytesSend;
    //Try to write the buffer on the Serial port
    std::cout<<", "<<sizeof buffer<<" bytes";
    if (!WriteFile(hSerial, buffer, nbChar, &bytesSend, nullptr)) {
        ClearCommError(hSerial, &errors, &status);
        return false;
    }
    else
        return true;
}

Arduino::~Arduino() {
    if (connected) {
        connected = false;
        CloseHandle(hSerial);
    }
}