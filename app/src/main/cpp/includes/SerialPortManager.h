//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SERIALPORTMANAGER_H
#define MSERIALPORT_SERIALPORTMANAGER_H

#include <unordered_map>
#include <IWorker.h>
#include <PFBackgroundService.h>

class SerialPortManager {
public:
    explicit SerialPortManager();

    virtual ~SerialPortManager();

    int addSerialPort(std::string &path, int flag, IWorker worker);

    int removeSerialPort(std::string &path, int flag);

    int sendMessage(const std::string &path, const std::string &msg, int flag);

    static const int FLAG_READ = 1;
    static const int FLAG_WRITE = 2;
private:
    std::unordered_map<std::string, std::unique_ptr<PFBackgroundService>> read_map;
    std::unordered_map<std::string, std::unique_ptr<PFBackgroundService>> write_map;


};

#endif //MSERIALPORT_SERIALPORTMANAGER_H
