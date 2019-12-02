//
// Created by Administrator on 2019/11/18.
//

#ifndef MSERIALPORT_IWORKER_H
#define MSERIALPORT_IWORKER_H

#include <jni.h>
#include <vector>
#include <string>
#include <future>

class IWorker {

public:
    IWorker() : futureObj(exitSignal.get_future()) {}

    virtual void doWork(const std::vector<std::string> &msgs) = 0;

    virtual void doWork(const std::vector<char>& msg) = 0;

    virtual ~IWorker() {}

    // Request the thread to stop by setting value in promise object
    virtual void stop() {
        exitSignal.set_value();
    }

    //Checks if thread is requested to stop
    bool stopRequested() {
        // checks if value in future object is available
        return !(futureObj.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout);
    }

private:
    std::promise<void> exitSignal;
    std::future<void> futureObj;
};

#endif //MSERIALPORT_IWORKER_H
