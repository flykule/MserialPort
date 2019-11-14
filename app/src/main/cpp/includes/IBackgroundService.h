//
// Created by Administrator on 2019/11/14.
//

#ifndef MSERIALPORT_IBACKGROUNDSERVICE_HPP
#define MSERIALPORT_IBACKGROUNDSERVICE_HPP

#include <string>

//After instantiation, objects manage a separate thread that can receive messages.
class IBackgroundService
{
public:
    //send msg to the thread this instance manages
    virtual void processMessage(std::string msg) = 0;
    //to follow RAII, the destructor sends a stop signal (via message) to the managed thread,
    //and can either wait or detach for the thread to exit
    virtual ~IBackgroundService() {}
};

#endif //MSERIALPORT_IBACKGROUNDSERVICE_HPP
