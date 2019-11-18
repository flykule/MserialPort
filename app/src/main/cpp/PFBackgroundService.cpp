#include <PFBackgroundService.h>

//const std::string PFBackgroundService::DESTROY= "destroy";

//template<typename W>
//PFBackgroundService::PFBackgroundService(W &worker) {
//    m_thread = std::make_unique<std::thread>(
//            [&](W &worker1) {
//                std::string msg;
//                while (true) {
//                    if (m_PF.ready()) {
//                        msg = m_PF.get();
//                        if (msg == PFBackgroundService::STOP) {
//                            worker1.interrupt();
//                            break;
//                        }
//                        worker1.doWork(msg);
//                    }
//                }
//            }, worker);
//}

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
    } catch (...) {
        //todo add some log here, but nothing to do now
    }

}


