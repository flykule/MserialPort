//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_SPWRITEWORKER_H
#define MSERIALPORT_SPWRITEWORKER_H

#include "IWorker.h"
#include "SerialPort.hpp"
#include <PFBackgroundService.h>

using namespace mn::CppLinuxSerial;

class SPWriteWorker : public IWorker {
public:
    SPWriteWorker(const char *c_name,const int* baudrate);
    void doWork(std::string &msg);
    virtual ~SPWriteWorker();
private:
    SerialPort* _serialPort;
};


#endif //MSERIALPORT_SPWRITEWORKER_H
