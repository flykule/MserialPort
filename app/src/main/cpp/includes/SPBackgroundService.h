//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SPBACKGROUNDSERVICE_H
#define MSERIALPORT_SPBACKGROUNDSERVICE_H

#include <PFBackgroundService.h>
#include <string>
#include <SerialPort.hpp>

using namespace mn::CppLinuxSerial;

class SPBackgroundService {
public:
    SPBackgroundService( std::string &name,
                        BaudRate baudRate);

    virtual ~SPBackgroundService();

    void processMsg(std::string& msg);

private:
    SerialPort _serialPort;
    std::unique_ptr<PFBackgroundService> _service;
};


#endif //MSERIALPORT_SPBACKGROUNDSERVICE_H
