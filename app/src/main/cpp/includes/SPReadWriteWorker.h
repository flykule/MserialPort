//
// Created by Administrator on 2019/11/25.
//

#ifndef MSERIALPORT_SPREADWRITEWORKER_H
#define MSERIALPORT_SPREADWRITEWORKER_H

#include "../includes/IWorker.h"
#include "SerialPort.hpp"
#include <unistd.h>
#include <queue>
#include <poll.h>

using namespace mn::CppLinuxSerial;
static constexpr auto START_READ = "start_read";

class SPReadWriteWorker : public IWorker {

    template<typename T>
    struct PromiseAndFuture {
        std::unique_ptr<std::promise<T>> m_promise;
        std::unique_ptr<std::future<T>> m_future;
        std::mutex m_mutex;

        //constructor
        PromiseAndFuture() {
            reset();
        }

        //reset the managed promise and future objects to new ones
        void reset() {
            m_promise.reset(nullptr);
            m_future.reset(nullptr);
            m_promise = std::make_unique<std::promise<T>>();
            m_future = std::make_unique<std::future<T>>(m_promise->get_future());
        }

        //non-blocking function that returns whether or not the
        //managed future object is ready, i.e., has a valid value
        bool ready() {
            std::future_status status = m_future->wait_for(std::chrono::milliseconds(0));
            return (status == std::future_status::ready);
        }

        //blocking function that retrieves and returns value in
        //managed future object
        T get() {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            T ret = m_future->get();
            reset();
            return ret;
        }

        //set the value on the managed promise object to val,
        //thereby making the future "ready" and contain val
        bool set(T val) {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            //don't
            if (ready()) {
                return false;
            }
            m_promise->set_value(val);
            return true;
        }
    };

    void readLoop();

    void stop() override {
        const std::lock_guard<std::mutex> lock(m_mutex);
        IWorker::stop();
        cv.notify_all();
    }

    void writeLoop();


private:
    void writeMessage(const std::vector<std::string> &messages);

    //instance of promise/future pair that is used for messaging
    static constexpr auto read_interval = 5000;
    std::mutex m_mutex;
    std::thread *read_thread;
    std::thread *write_thread;
    std::queue<std::vector<std::string>> mMessages;
    std::queue<std::vector<char>> mByteMessages;
    JavaVM *g_vm;
    jobject *jcallback;
    JNIEnv *env;
    SerialPort *_serialPort;
    std::condition_variable cv;
public:
    SPReadWriteWorker(std::string &name, const int &baudrate, JavaVM *vm, jobject *callback);

    virtual ~SPReadWriteWorker();

    void doWork(const std::vector<std::string> &msgs) override;

    virtual void doWork(const std::vector<char> &msgs) override;
};


#endif //MSERIALPORT_SPREADWRITEWORKER_H
