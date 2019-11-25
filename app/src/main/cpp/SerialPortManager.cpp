//
// Created by Administrator on 2019/11/14.
//

#include "includes/SerialPortManager.h"

SerialPortManager::SerialPortManager() = default;

int SerialPortManager::removeSerialPort(std::string path) {
    if (inner_map[path]) {
        inner_map[path].reset(nullptr);
        inner_map.erase(path);
        return 0;
    } else {
        return -1;
    }
}

int
SerialPortManager::sendMessage(std::string path, const std::vector<std::string> &msg) {
    if (inner_map[path]) {
        inner_map[path]->
                doWork(msg);
        return 0;
    } else {
        return -1;
    }
}

SerialPortManager::~SerialPortManager() {
    for (auto &r:inner_map) {
        removeSerialPort(r.first);
    }
}

bool SerialPortManager::hasSerialPort(std::string path) {
    return inner_map.find(path) != inner_map.end();
}



