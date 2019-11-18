//
// Created by Administrator on 2019/11/14.
//

#include "includes/SerialPortManager.h"

SerialPortManager::SerialPortManager() = default;

int SerialPortManager::removeSerialPort(std::string path, int flag) {
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

int SerialPortManager::sendMessage(std::string path, const std::string &msg, int flag) {
    if (flag & FLAG_READ) {
        LOGD("发送读数据%s到%s", msg.c_str(), path.c_str());
        read_map[path]->processMessage(msg);
    }
    if (flag & FLAG_WRITE) {
        LOGD("发送写数据%s到%s", msg.c_str(), path.c_str());
        write_map[path]->processMessage(msg);
    }
    return 0;
}

SerialPortManager::~SerialPortManager() {
    for (auto &r:read_map) {
        removeSerialPort(r.first, FLAG_READ);
    }
    for (auto &r:write_map) {
        removeSerialPort(r.first, FLAG_WRITE);
    }
}


