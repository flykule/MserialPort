//
// Created by Administrator on 2019/11/14.
//

#include "includes/SerialPortManager.h"

SerialPortManager::SerialPortManager() = default;

int SerialPortManager::addSerialPort(std::string &path, int flag, IWorker worker) {
    if (flag & FLAG_READ) {
        read_map[path] == std::make_unique<PFBackgroundService>(path, worker);
    }
    if (flag & FLAG_WRITE) {
        write_map[path] == std::make_unique<PFBackgroundService>(path, worker);
    }
    return 0;
}

int SerialPortManager::removeSerialPort(std::string &path, int flag) {
    if (flag & FLAG_READ) {
        read_map[path]->processMessage(PFBackgroundService::STOP);
        read_map.erase(path);
    }
    if (flag & FLAG_WRITE) {
        write_map[path]->processMessage(PFBackgroundService::STOP);
        write_map.erase(path);
    }
    return 0;

}

int SerialPortManager::sendMessage(const std::string &path, const std::string &msg, int flag) {
    if (flag & FLAG_READ) {
        read_map[path]->processMessage(path);
    }
    if (flag & FLAG_WRITE) {
        write_map[path]->processMessage(path);
    }
    return 0;
}

SerialPortManager::~SerialPortManager() {
    for (auto r:read_map) {
        removeSerialPort(r.first, FLAG_READ);
    }
    for (auto r:write_map) {
        removeSerialPort(r.first, FLAG_WRITE);
    }
}


