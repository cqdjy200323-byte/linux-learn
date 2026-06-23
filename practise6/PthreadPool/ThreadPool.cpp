#include <iostream>
#include <unistd.h>
#include <queue>
#include <pthread.h>

#define NUM 5

template<class T>
class ThreadPool
{
private:
    bool IsEmpty()
    {
        return _task_queue.size() == 0;
    }

    void LockQueue()
    {
        pthread_mutex_lock(&_mutex);
    }

    void UnlockQueue()
    {
        pthread_mutex_unlock(&_mutex);
    }

    void Wait()
    {
        pthread_cond_wait(&_cond,&_mutex);
    }

    void WakeUp()
    {
        pthread_cond_signal(&_cond,&_mutex)
    }

public:
    ThreadPool(const int num)
    :_thread_num(num)
    {
        pthread_mutex_init(&_mutex);
        pthread_cond_init(&_cond);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);

    }

    static void* Routine(void* arg)
    {
        pthread_detach(pthread_self());
        ThreadPool<T>* Self=(ThreadPool<T>*) arg;

        while(true)
        {
            Self->LockQueue();
            while(Self->IsEmpty)
            {
                Self->Wait();
            }
            T task;
            Self->Pop(task);
            Self->UnlockQueue();
            task.Run();
        }
    }

    ThreadPoolInit()
    {
        pthread_t pt;
        for (int i = 0; i < _thread_num;i++)
        {
            pthread_create(&pt, nullptr, Routine, this);
        }
    }

    void Push(T task)
    {
        LockQueue();
        _task_queue.push(task);
        UnlockQueue();
        WakeUp();
    }

    void Pop(T& task)
    {
        task = _task_queue.front();
        _task_queue.pop();
    }

private:
    std::queue<T> _task_queue;
    int _thread_num;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};
