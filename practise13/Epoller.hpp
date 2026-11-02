#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include "Common.hpp"
#include "Log.hpp"

using namespace LogModule;

class Epoller
{
public:
    Epoller():_epfd(-1)
    {
        _epfd = epoll_create(128);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error!!!";
            exit(EPOLL_CREATE_ERR);
        }
        LOG(LogLevel::INFO) << "create epoll success: " << _epfd;
    }

    void Wait()
    {
        
    }
    void AddEvent(int sockfd,uint32_t event)
    {
        struct epoll_event ev;
        ev.data.fd = sockfd;
        ev.events=event;
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &ev);
        if(n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_ctl error";
            return;
        }
        LOG(LogLevel::INFO) << "epoll_ctl success: " << sockfd;
    }

    void DelEvent()
    {

    }

    void ModEvent()
    {

    }



     ~Epoller()
    {
        if(_epfd >= 0)
        {
            close(_epfd);
        }
    }

private:
    int _epfd;
};