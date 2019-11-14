//
// Created by Administrator on 2019/11/14.
//

#include "includes/SerialPortManager.h"

SerialPortManager::SerialPortManager() : _map(std::unordered_map()) {

}

SerialPortManager::~SerialPortManager() {
    closeAll();
}

int SerialPortManager::addSerialPort(std::string &path, BaudRate baudRate) {
    auto search = _map.find(path);
    if (search == _map.end()) {
//        std::unique_ptr<IBackgroundService> pfService = std::make_unique<PFBackgroundService>(
//                [](std::string msg) {
//                    LOGD("Received message %s", msg.c_str());
//                });
//        _map.insert(path,)
    }
    return 0;
}
