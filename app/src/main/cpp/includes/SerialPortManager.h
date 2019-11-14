//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SERIALPORTMANAGER_H
#define MSERIALPORT_SERIALPORTMANAGER_H

#include <SerialPort.hpp>
#include "SerialPort.hpp"
#include <unordered_map>
#include <string>
#include <PFBackgroundService.h>

using namespace mn::CppLinuxSerial;

class SerialPortManager {
public:
    explicit SerialPortManager();

    virtual ~SerialPortManager();

    int addSerialPort(std::string& path, BaudRate baudRate);

    void removeSerialPort(std::string& path);

    int sendMessage(std::string& path, const std::string& msg);

private:
    std::unordered_map<std::string, std::unique_ptr<IBackgroundService>> _map;

    void closeAll();

};

#endif //MSERIALPORT_SERIALPORTMANAGER_H
