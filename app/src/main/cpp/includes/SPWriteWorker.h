//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_SPWRITEWORKER_H
#define MSERIALPORT_SPWRITEWORKER_H

#include "IWorker.h"
#include "SerialPort.hpp"

using namespace mn::CppLinuxSerial;

class SPWriteWorker : public IWorker {
public:
    SPWriteWorker(const char *c_name,const int* baudrate);
    void doWork(const std::vector<std::string>& msgs) override ;
    virtual ~SPWriteWorker();
    virtual void doWork(const std::vector<char>& msg) override {};
private:
    void internalWork(std::string& msg);
    SerialPort* _serialPort;
    std::mutex m_mutex;
};


#endif //MSERIALPORT_SPWRITEWORKER_H
