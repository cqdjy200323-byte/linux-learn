#ifndef __PROCESS_POOL_HPP__
#define __PROCESS_POOL_HPP__

#include <iostream>
#include <cstdlib> // stdlib.h stdio.h -> cstdlib cstdio
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include "Task.hpp"

class Channel
{
public:
    Channel(int wfd, pid_t pid)
    :_wfd(wfd)
    ,_subid(pid)
    {
        _name = "channel" + std::to_string(_wfd) + "-" + std::to_string(_subid);
    }

    ~Channel()
    {
    }

    void Send(int code)
    {
        int n = write(_wfd, &code, sizeof(code));
        (void)n;
    }

    void Stop()
    {
        close(_wfd);
    }

    void Wait()
    {
        pid_t pid = waitpid(_subid, nullptr, 0);
        (void)pid;
    }

private:
    int _wfd;
    pid_t _subid;
    std::string _name;

};

class ChannelManager
{
public:
    ChannelManager()
        :_next(0)
    {
    }


    ~ChannelManager()
    {
    }

    void Insert(int wfd, int pid)
    {
        _Channels.emplace_back(wfd,pid);
    }

    Channel& Select()
    {
        _next %= _Channels.size();
        return _Channels[_next++];
    }

    void StopSubProcess()
    {
        for (auto &channel : _Channels)
        {
            channel.Stop();
        }
    }
    void WaitSubProcess()
    {
        for (auto &channel : _Channels)
        {
            channel.Wait();
        }
    }



private:
    std::vector<Channel> _Channels;
    int _next;

};


class ProcessPool
{
public:
    ProcessPool(int size)
        :_process_size(size)
    {
        _tm.Register(PrintLog);
        _tm.Register(Download);
        _tm.Register(Upload);
    }

    ~ProcessPool()
    {

    }

    void Work(int rfd)
    {
        while (true)
        {
            int code = 0;
            ssize_t n = read(rfd, &code, sizeof(code));
            if (n > 0)
            {
                if (n != sizeof(code))
                {
                    continue;
                }
                std::cout << "子进程[" << getpid() << "]收到一个任务码: " << code << std::endl;
                _tm.Execute(code);
            }
            else if (n == 0)
            {
                std::cout << "子进程退出" << std::endl;
                break;
            }
            else
            {
                std::cout << "读取错误" << std::endl;
                break;
            }
        }


    }


    bool Start()
    {
        for (int i = 0;i < _process_size;i++)
        {
            int pipefd[2] = { 0 };
            int n = pipe(pipefd);
            if (n < 0)
            {
                return false;
            }
            pid_t subid = fork();
            if (subid < 0)
            {
                return false;
            }
            else if (subid == 0)
            {
                close(pipefd[1]);
                Work(pipefd[0]);
                close(pipefd[0]);
                exit(0);
            }
            else
            {
                close(pipefd[0]);
                _cm.Insert(pipefd[1], subid);
            }
        }
        return true;
    }

    void Run()
    {
        int taskcode = _tm.Code();
        auto& c = _cm.Select();

        c.Send(taskcode);
    }

    void Stop()
    {
        _cm.StopSubProcess();
        _cm.WaitSubProcess();
    }

private:
    ChannelManager _cm;
    int _process_size;
    TaskManager _tm;

};

#endif