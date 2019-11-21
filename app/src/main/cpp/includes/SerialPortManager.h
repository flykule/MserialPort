//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SERIALPORTMANAGER_H
#define MSERIALPORT_SERIALPORTMANAGER_H

#include <unordered_map>
#include <SPWriteWorker.h>
#include <SPReadWorker.h>
#include <PFBackgroundService.h>
#include <androidLog.h>

class SerialPortManager {
public:
    explicit SerialPortManager();

    virtual ~SerialPortManager();

    template<typename T>
    int addSerialPort(const char *path, int flag, T *worker) {
        if (flag & FLAG_READ) {
            read_map[path] = std::make_unique<PFBackgroundService>(worker);
            LOGD("添加读串口%s", path);
        }
        if (flag & FLAG_WRITE) {
            write_map[path] = std::make_unique<PFBackgroundService>(worker);
            LOGD("添加写串口%s", path);
        }
        return 0;
    }

    int removeSerialPort(std::string path, int flag);

    int sendMessage(std::string path, const std::vector<std::string> msg, int flag);


    static constexpr unsigned int FLAG_WRITE{0x1}; // hex for 0000 0001
    static constexpr unsigned int FLAG_READ{0x2}; // hex for 0000 0010
private:
    std::unordered_map<std::string, std::unique_ptr<PFBackgroundService>> read_map;
    std::unordered_map<std::string, std::unique_ptr<PFBackgroundService>> write_map;

};

#endif //MSERIALPORT_SERIALPORTMANAGER_H
