//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SERIALPORTMANAGER_H
#define MSERIALPORT_SERIALPORTMANAGER_H

#include <unordered_map>
#include <SPBackgroundService.h>

using namespace mn::CppLinuxSerial;

class SerialPortManager {
public:
    explicit SerialPortManager();

    virtual ~SerialPortManager();

    int addSerialPort(std::string &path, int baudRate);
    SerialPort& getSerialPort(std::string &path);

    int addSerialPort(const std::function<void(std::string)> &reactor, std::string &path, int baudRate);

    int removeSerialPort(std::string &path);

    int sendMessage(const std::string &path, const std::string &msg);

private:
    std::unordered_map<std::string, std::unique_ptr<SPBackgroundService>> _map;

    void closeAll();

};

#endif //MSERIALPORT_SERIALPORTMANAGER_H
