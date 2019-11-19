//
// Created by Administrator on 2019/11/14.
//

#include "includes/SerialPortManager.h"

SerialPortManager::SerialPortManager() = default;

int SerialPortManager::removeSerialPort(const char* path, int flag) {
    if (flag & FLAG_READ) {
        if (read_map[path]) {
            read_map[path].reset(nullptr);
            read_map.erase(path);
        }
    }
    if (flag & FLAG_WRITE) {
        if (write_map[path]) {
            write_map[path].reset(nullptr);
            write_map.erase(path);
        }
    }
    return 0;

}

int SerialPortManager::sendMessage(const char* path, const std::string &msg, int flag) {
    if (flag & FLAG_READ) {
//        LOGD("发送读数据%s到%s", msg.c_str(), path.c_str());
        if (read_map[path])
            read_map[path]->processMessage(msg);
    }
    if (flag & FLAG_WRITE) {
//        LOGD("发送写数据%s到%s", msg.c_str(), path.c_str());
        if (write_map[path])
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


