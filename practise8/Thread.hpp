#ifndef _THREAD_H_
#define _THREAD_H_

#include <iostream>
#include <string>
#include <pthread.h>
#include <cstdio>
#include <cstring>
#include <functional>
#include<cstdint>

namespace ThreadModlue
{
    static uint32_t number = 1;

    class Thread
    {
        using func_t = std::function<void()>;

    private:
        void EnableDetach()
        {
            _isdetach = true;
        }
        void EnableRunning()
        {
            _isrunning = true;
        }

        static void* Routine(void* args)
        {
            Thread *self = static_cast<Thread *>(args);
            self->EnableRunning();
            if (self->_isdetach)
                self->Detach();
            pthread_setname_np(self->_tid, self->_name.c_str());
            self->_func(); // 回调处理
            return nullptr;
        }

    public:
        Thread(func_t func)
        : _tid(0)
        ,_isdetach(false)
        ,_isrunning(false)
        ,ret(nullptr)
        ,_func(func)
        {
            _name = "thread-" + std::to_string(number++);
        }

        void Detach()
        {
            if(_isdetach)
                return;
            if(_isrunning)
            pthread_detach(_tid);
            EnableDetach();
        }


        bool Start()
        {
            if(_isrunning)
            {
                return false;
            }
            int n = pthread_create(&_tid, nullptr, Routine, this);
            if (n != 0)
            {
                return false;
            }
            else
            {
                return true;
            }

        }
        void Join()
        {
            if(_isdetach)
                return;
            int n = pthread_join(_tid, &ret);
            if(n!=0)
            {

            }
            else
            {

            }
        }

        pthread_t Id()
        {
            return _tid;
        }

        std::string Name()
        {
            return _name;
        }

        ~Thread()
        {}

    private:
        pthread_t _tid;
        bool _isdetach;
        bool _isrunning;
        std::string _name;
        func_t _func;
        void *ret;
    };
}

#endif