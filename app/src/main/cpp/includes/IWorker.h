//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_IWORKER_H
#define MSERIALPORT_IWORKER_H

#include <jni.h>
#include <string>

class IWorker {

public:
    IWorker() : interrupted(0) {}

    virtual void doWork(std::string &msg) = 0;

    virtual ~IWorker() {}

    virtual void stop() {
        interrupted = -1;
    }

    virtual int isInterrupted() {
        return interrupted;
    }

private:
    int interrupted;
};

#endif //MSERIALPORT_IWORKER_H
