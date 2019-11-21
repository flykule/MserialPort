#include <PFBackgroundService.h>

void PFBackgroundService::processMessage(const std::vector<std::string> msgs) {
    bool success = false;
    while (!success) {
        success = m_PF.set(msgs);
    }
}

PFBackgroundService::~PFBackgroundService() {
    try {
        LOGD("开始销毁后台线程");
        processMessage({PFBackgroundService::STOP});
        if (m_thread->joinable()) {
            m_thread->join();
        }
        delete _worker;
        _worker = nullptr;
        LOGD("销毁后台线程");
    } catch (...) {
        //todo add some log here, but nothing to do now
    }

}


