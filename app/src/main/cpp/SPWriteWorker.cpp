//
// Created by Administrator on 2019/11/18.
//

#include <SPWriteWorker.h>
#include <androidLog.h>

const int BIT16 = 16;

static void HexToBytes(const std::string &hex, char *result) {
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char) strtol(byteString.c_str(), nullptr, BIT16);
        *result = byte;
        result++;
    }
}

SPWriteWorker::SPWriteWorker(const char *c_name, const int *baudrate) :
        _serialPort(new SerialPort(c_name, *baudrate)) {
    _serialPort->Open();
    LOGD("打开串口%s成功", c_name);
}

SPWriteWorker::~SPWriteWorker() {
//    if (_serialPort) {
//        _serialPort->Close();
//        _serialPort == nullptr;
//    }
    LOGD("worker关闭串口成功");
};

void SPWriteWorker::doWork(std::string &msg) {
    LOGD("开始写数据%s", msg.c_str());
    if(_serialPort->currendState()!=mn::CppLinuxSerial::State::OPEN){
        LOGE("串口关闭状态,直接返回");
        return;
    }
    int len = msg.length() / 2;
    char temp[len];
    HexToBytes(msg, temp);
    _serialPort->Write(temp, len);
}
