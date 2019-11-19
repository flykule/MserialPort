//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_SPREADWORKER_H
#define MSERIALPORT_SPREADWORKER_H

#include <SerialPort.hpp>
#include "IWorker.h"
#include <PFBackgroundService.h>

using namespace mn::CppLinuxSerial;
class SPReadWorker : IWorker {
public:
    void stop();
    SPReadWorker(const char *c_name,const int* baudrate, JavaVM *vm,jobject *callback,jmethodID *methodId);
    virtual ~SPReadWorker();
    void doWork(std::string &msg);

private:
    JavaVM *g_vm;
    jmethodID* _method_id;
    jobject* jcallback;
    JNIEnv* env;
    SerialPort* _serialPort;
};


#endif //MSERIALPORT_SPREADWORKER_H
