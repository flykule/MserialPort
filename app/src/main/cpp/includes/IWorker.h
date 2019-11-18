//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_IWORKER_H
#define MSERIALPORT_IWORKER_H

#include <string>

class IWorker {

public:
    template<typename ...types>
    virtual void doWork(std::string &msg, types...args) = 0;

    virtual ~IWorker() {}

    void interrupt() {
        interrupted = -1;
    }

    int isInterrupted(){
        return interrupted;
    }

private:
    int interrupted;
};

#endif //MSERIALPORT_IWORKER_H
