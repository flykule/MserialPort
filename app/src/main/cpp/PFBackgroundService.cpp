#include <PFBackgroundService.h>

const std::string PFBackgroundService::STOP = "stop";
//const std::string PFBackgroundService::DESTROY= "destroy";

template<typename ...typs>
PFBackgroundService::PFBackgroundService(const IWorker& worker, typs...args)
{
    m_thread = std::make_unique<std::thread>(
        [&](IWorker& worker1)
        {
            std::string msg;
            while (true)
            {
                if (m_PF.ready())
                {
                    msg = m_PF.get();
                    if (msg == PFBackgroundService::STOP)
                    {
                        worker1.interrupt();
                        break;
                    }
                    worker1.doWork(msg,args);
//                    if (msg == PFBackgroundService::DESTROY)
//                    {
//                        break;
//                    }
                }
            }            
        }, worker);
}

void PFBackgroundService::processMessage(std::string msg)
{
    bool success = false;
    while (!success)
    {
        success = m_PF.set(msg);
    }
}

PFBackgroundService::~PFBackgroundService()
{
    processMessage(PFBackgroundService::STOP);
    if (m_thread->joinable())
    {
        m_thread->join();
    }
}


