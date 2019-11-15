//
// Created by Administrator on 2019/11/14.
//

#include "includes/SerialPortManager.h"


SerialPortManager::SerialPortManager() = default;

SerialPortManager::~SerialPortManager() {
    closeAll();
}

int SerialPortManager::addSerialPort(std::string &path, int baudRate) {
    _map[path] = std::make_unique<SPBackgroundService>(path, baudRate);
    return 0;
}

int
SerialPortManager::addSerialPort(const std::function<void(std::string)> &reactor, std::string &path,
                                 int baudRate) {
    _map[path] = std::make_unique<SPBackgroundService>(path, baudRate, reactor);
    return 0;
}

int SerialPortManager::removeSerialPort(std::string &path) {
    auto search = _map.find(path);
    if (search == _map.end()) {
        return -1;
    }
    _map.erase(search);
    return 0;

}

int SerialPortManager::sendMessage(const std::string &path, const std::string &msg) {
    auto search = _map.find(path);
    if (search == _map.end()) {
        return -1;
    }
    _map[path]->processMsg(msg);
    return 0;
}

void SerialPortManager::closeAll() {
    _map.clear();
}

SerialPort& SerialPortManager::getSerialPort(std::string &path) {
    return _map[path]->getSerialPort();
}


