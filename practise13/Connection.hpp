#include<iostream>
#include<memory>
#include"InetAddr.hpp"

class Reactor;


using handler_t = std::function<std::string (std::string &)>;

class Connection
{
public:
    Connection()
    {

    }

    virtual void Recver() = 0;
    virtual void Send() = 0;
    virtual void Excepter() = 0;
    virtual int GetSockFd() = 0;

    void SetEvent(const uint32_t& event)
    {
        _events = event;
    }
    uint32_t GetEvent()
    {
        return _events;
    }

    void SetOwner(Reactor* owner)
    {
        _owner = owner;
    }

    Reactor *GetOwner()
    {
        return _owner;
    }

    ~Connection()
    {

    }

private:
    
    Reactor *_owner;
    uint32_t _events;

public:
    handler_t _handler;
};