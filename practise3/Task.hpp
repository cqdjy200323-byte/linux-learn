#pragma once

#include <iostream>
#include <vector>
#include <ctime>

typedef void (*task_t)();

////////////////debug/////////////////////
void PrintLog()
{
    std::cout << "我是一个打印日志的任务" << std::endl;
}

void Download()
{
    std::cout << "我是一个下载的任务" << std::endl;
}

void Upload()
{
    std::cout << "我是一个上传的任务" << std::endl;
}


class TaskManager
{
public:

    TaskManager()
    {
        srand(time(nullptr));
    }

    ~TaskManager()
    {

    }

    void Register(task_t t)
    {
        _task.push_back(t);
    }

    void Execute(int code)
    {
        _task[code]();
    }

    int Code()
    {
        return rand() % _task.size();
    }
private:
    std::vector<task_t> _task;
};