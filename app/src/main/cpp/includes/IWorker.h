//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_IWORKER_H
#define MSERIALPORT_IWORKER_H

#include <jni.h>
#include <string>

class IWorker {

public:
    IWorker() {}

    virtual void doWork(std::string &msg) = 0;

    virtual ~IWorker() {}

    void stop() {
        interrupted = -1;
    }

    int isInterrupted() {
        return interrupted;
    }

private:
    int interrupted;
};

#endif //MSERIALPORT_IWORKER_H
