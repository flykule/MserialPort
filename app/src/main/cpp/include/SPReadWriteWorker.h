//
// Created by Administrator on 2019/11/25.
//

#ifndef MSERIALPORT_SPREADWRITEWORKER_H
#define MSERIALPORT_SPREADWRITEWORKER_H

#include "../includes/IWorker.h"
#include <SerialPort.hpp>
#include <unistd.h>

using namespace mn::CppLinuxSerial;

class SPReadWriteWorker : public IWorker {

    void doWork(const std::vector<std::string> &msgs) override;

    SPReadWriteWorker(std::string &name, const int &baudrate, JavaVM *vm, jobject *callback);

    void readLoop();

    virtual ~SPReadWriteWorker();

private:
    void writeMessage(const std::string& msg);

    static constexpr auto read_interval = 200000;
    std::mutex m_mutex;
    std::thread *work_thread;
    JavaVM *g_vm;
    jobject *jcallback;
    JNIEnv *env;
    SerialPort *_serialPort;
};


#endif //MSERIALPORT_SPREADWRITEWORKER_H
