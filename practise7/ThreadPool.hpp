#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Log.hpp"
#include "Thread.hpp"
#include "Cond.hpp"
#include "Mutex.hpp"

namespace ThreadPoolModule
{
    using namespace ThreadModlue;
    using namespace LogModule;
    using namespace CondModule;
    using namespace MutexModule;

    static const int gnum = 5;
    template <class T>
    class ThreadPool
    {
    private:
        void WakeUpAllThread()
        {
            LockGuard lockguard(_mutex);
            if (_sleepernum)
                _cond.Broadcast();
            LOG(LogLevel::INFO) << "唤醒所有的休眠线程";
        }

        void WakeUpOne()
        {
            _cond.Signal();
            LOG(LogLevel::INFO) << "唤醒一个休眠线程";
        }

        ThreadPool(int num = gnum)
            : _thread_num(num), _isrunning(false), _sleepernum(0)
        {
            for (int i = 0; i < _thread_num; i++)
            {
                _threads.emplace_back(
                    [this]()
                    { HandlerTask(); });
            }
        }

        void Start()
        {
            if (_isrunning)
                return;
            _isrunning = true;
            for (auto &thread : _threads)
            {
                thread.Start();
                LOG(LogLevel::INFO) << "start new thread success: " << thread.Name();
            }
        }

        ThreadPool(const ThreadPool<T> &) = delete;
        ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

    public:
        static ThreadPool<T> *GetInstance()
        {
            if (_inc == nullptr)
            {
                LockGuard lockguard(_lock);
                LOG(LogLevel::DEBUG) << "获取单例....";
                if (_inc == nullptr)
                {
                    LOG(LogLevel::DEBUG) << "首次使用单例, 创建之....";
                    _inc = new ThreadPool<T>();
                    _inc->Start();
                }
            }
            return _inc;
        }
        void Stop()
        {
            if (!_isrunning)
                return;
            _isrunning = false;

            // 唤醒所有的线层
            WakeUpAllThread();
        }
        void Join()
        {
            for (auto &thread : _threads)
            {
                thread.Join();
            }
        }

        void HandlerTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));

            while (true)
            {
                T t;
                {
                    LockGuard lockguard(_mutex);
                    while ( _task_queue.empty()&&_isrunning)
                    {
                        _sleepernum++;
                        _cond.Wait(_mutex);
                        _sleepernum--;
                    }

                    if (!_isrunning && _task_queue.empty())
                    {
                        LOG(LogLevel::INFO) << name << " 退出了, 线程池退出&&任务队列为空";
                        break;
                    }

                    t = _task_queue.front();
                    _task_queue.pop();
                }
                t();
            }
        }
        bool Enqueue(const T &in)
        {
            if (_isrunning)
            {
                LockGuard lockguard(_mutex);
                _task_queue.push(in);
                if (_threads.size() == _sleepernum)
                    WakeUpOne();
                return true;
            }
            return false;
        }

    private:
        std::vector<Thread> _threads;
        std::queue<T> _task_queue;
        int _thread_num;
        Cond _cond;
        Mutex _mutex;
        bool _isrunning;
        int _sleepernum;
        static ThreadPool<T> *_inc;
        static Mutex _lock;
    };

    template <typename T>
    ThreadPool<T> *ThreadPool<T>::_inc = nullptr;

    template <typename T>
    Mutex ThreadPool<T>::_lock;
}
