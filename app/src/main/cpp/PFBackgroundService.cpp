#include <PFBackgroundService.h>

const std::string PFBackgroundService::STOP = "stop";
const std::string PFBackgroundService::DESTROY= "destroy";

PFBackgroundService::PFBackgroundService(const std::function<void(std::string)>& reactor)
{
    m_thread = std::make_unique<std::thread>(
        [&](std::function<void(std::string)> reactor)
        {
            std::string msg;
            while (true)
            {
                if (m_PF.ready())
                {
                    msg = m_PF.get();
                    if (msg == PFBackgroundService::STOP)
                    {
                        break;
                    }
                    reactor(msg);
                    if (msg == PFBackgroundService::DESTROY)
                    {
                        break;
                    }
                }
            }            
        }, reactor);
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


