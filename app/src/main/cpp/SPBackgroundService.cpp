//
// Created by Administrator on 2019/11/14.
//

#include "includes/SPBackgroundService.h"
#include <androidLog.h>

SPBackgroundService::SPBackgroundService(std::string &name, int baudRate) :
        _serialPort(SerialPort(name, baudRate)) {
    _service = std::make_unique<PFBackgroundService>([this](std::string msg) {
//        LOGD("执行发送消息%s", msg.c_str());
        int len = msg.length() / 2;
        char temp[len];
        HexToBytes(msg, temp);
        _serialPort.Write(temp, len);
    });
    _serialPort.Open();
}

SPBackgroundService::~SPBackgroundService() {
    _service.reset(nullptr);
    _serialPort.Close();
}

void SPBackgroundService::processMsg(std::string &msg) {
    if (_serialPort.currendState() == State::OPEN && _service != nullptr) {
        _service->processMessage(msg);
    }
}

SPBackgroundService::SPBackgroundService(std::string &name, int baudRate,
                                         const std::function<void(std::string)> &reactor)
        : _serialPort(SerialPort(name, baudRate)),
        _service(std::make_unique<PFBackgroundService>((reactor)))
        { }

