//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_SPBACKGROUNDSERVICE_H
#define MSERIALPORT_SPBACKGROUNDSERVICE_H

#include <PFBackgroundService.h>
#include <string>
#include <SerialPort.hpp>

using namespace mn::CppLinuxSerial;

const int BIT16 = 16;
static void HexToBytes(const std::string &hex, char *result) {
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), nullptr, BIT16);
        *result = byte;
        result++;
    }
}
class SPBackgroundService {
public:
    SPBackgroundService( std::string &name,
                        int baudRate);
    SPBackgroundService(std::string &name, int baudRate,const std::function<void(std::string)>& reactor);

    virtual ~SPBackgroundService();

    void processMsg(std::string& msg);

private:
    SerialPort _serialPort;
    std::unique_ptr<PFBackgroundService> _service;
};


#endif //MSERIALPORT_SPBACKGROUNDSERVICE_H
