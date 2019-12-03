//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SERIALPORTMANAGER_H
#define MSERIALPORT_SERIALPORTMANAGER_H

#include <unordered_map>
#include <SPWriteWorker.h>
#include <SPReadWorker.h>
#include <androidLog.h>

class SerialPortManager {
public:
    explicit SerialPortManager();

    virtual ~SerialPortManager();

    bool hasSerialPort(std::string path);

    int addSerialPort(const char *path, std::unique_ptr<IWorker> worker) {
        inner_map[path] = std::move(worker);
        LOGD("添加串口%s", path);
        return 0;
    }

    int removeSerialPort(std::string path);

    int sendMessage(std::string path, const std::vector<std::string> &msg);

    int sendBytesMessage(std::string path, const std::vector<char> &msg);

private:
    std::unordered_map<std::string, std::unique_ptr<IWorker>> inner_map;

};

#endif //MSERIALPORT_SERIALPORTMANAGER_H
