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
    } else {
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
                printf("ALERT: Could not set Serial Port parameters");

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
            //We wait 2s as the arduino board will be resetting
            Sleep(2000);

        }
    }

}

void Arduino::SendCommand(int command) {
    command += 65;
    char com = command;
    const char *c_command = &com;
    std::cout << ", sending command: " << (char) command;

    if (!WriteData(c_command, 5))
        std::cout << ", data failed to send" << std::endl;
    else
        std::cout << ", data successfully sent" << std::endl;
}

void Arduino::SendLongCommand(int command, const char *data) {
    command += 65;
    char com = command;
    const char *c_command = &com;

    char buffer[256];
    strncpy(buffer, c_command, sizeof(buffer));
    strncat(buffer, data, sizeof(buffer));

    std::cout << ", sending command: " << (char) command;

    if (!WriteData(buffer, sizeof(buffer)))
        std::cout << ", data failed to send, ";
    else
        std::cout << ", data successfully sent, ";

    Sleep(500);

    if (!ReadData(buffer, 5))
        std::cout << "data failed to receive" << std::endl;
    else
        std::cout << "received data: " << buffer<<std::endl;
}

bool Arduino::ReadData(char *buffer, unsigned int nbChar) {
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(hSerial, &errors, &status);

    //Check if there is something to read
    if (this->status.cbInQue > 0) {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if (status.cbInQue > nbChar) {
            toRead = nbChar;
        } else {
            toRead = status.cbInQue;
        }
        //Try to read the require number of chars, and return the number of read bytes on success
        if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, nullptr)) {
            return true;
        }
    }

    //If nothing has been read, or that an error was detected return 0
    return false;
}

bool Arduino::WriteData(const char *buffer, unsigned int nbChar) {
    //WriteFile(hSerial, buffer, 1, &nbChar, NULL);
    DWORD bytesSend;
    //Try to write the buffer on the Serial port
    if (!WriteFile(hSerial, buffer, nbChar, &bytesSend, nullptr)) {
        ClearCommError(hSerial, &errors, &status);
        return false;
    } else
        return true;
}

Arduino::~Arduino() {
    //Check if we are connected before trying to disconnect
    if (connected) {
        //We're no longer connected
        connected = false;
        //Close the serial handler
        CloseHandle(hSerial);
    }
}