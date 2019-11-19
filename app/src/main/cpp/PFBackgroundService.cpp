#include <PFBackgroundService.h>

void PFBackgroundService::processMessage(std::string msg) {
    bool success = false;
    while (!success) {
        success = m_PF.set(msg);
    }
}

PFBackgroundService::~PFBackgroundService() {
    try {
        processMessage(PFBackgroundService::STOP);
        if (m_thread->joinable()) {
            m_thread->join();
        }
        LOGD("后台线程销毁完毕");
    } catch (...) {
        //todo add some log here, but nothing to do now
    }

}


