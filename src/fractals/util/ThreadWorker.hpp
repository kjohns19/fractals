#ifndef INCLUDED_THREAD_WORKER_HPP
#define INCLUDED_THREAD_WORKER_HPP

#include <fractals/util/Semaphore.hpp>
#include <thread>

class ThreadWorker
{
public:
    ThreadWorker();
    ~ThreadWorker();

    template<typename Func, typename... Args>
    void doWork(Func func, Args... args);
    template<typename Func, typename... Args>
    bool tryDoWork(Func func, Args... args);

    void wait();
private:
    void threadFunc();

    std::thread d_thread;
    std::function<void()> d_func;
    Semaphore d_semWork;
    Semaphore d_semDone;
    Semaphore d_semWait;
    bool d_killed;
};

template<typename Func, typename... Args>
void ThreadWorker::doWork(Func func, Args... args)
{
    d_semWait.tryWait();
    d_semDone.wait();
    d_func = std::bind(func, std::forward(args)...);;
    d_semWork.post();
}

template<typename Func, typename... Args>
bool ThreadWorker::tryDoWork(Func func, Args... args)
{
    if (d_semDone.tryWait())
    {
        d_func = std::bind(func, std::forward(args)...);
        d_semWork.post();
        return true;
    }
    return false;
}

#endif //INCLUDED_THREAD_WORKER_HPP
