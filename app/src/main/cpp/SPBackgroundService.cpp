//
// Created by Administrator on 2019/11/14.
//

#include "includes/SPBackgroundService.h"

SPBackgroundService::SPBackgroundService(std::string &name, int baudRate) :
        _serialPort(SerialPort(name, baudRate)) {
    _service = std::make_unique<PFBackgroundService>([this](std::string msg) {
        char temp[msg.size() / 2];
        HexToBytes(msg, temp);
        _serialPort.Write(temp);
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

