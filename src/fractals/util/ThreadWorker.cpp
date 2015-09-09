#include <fractals/util/ThreadWorker.hpp>
#include <iostream>

ThreadWorker::ThreadWorker()
: d_thread()
, d_semDone(1)
, d_killed(false)
{
    d_thread = std::thread(&ThreadWorker::threadFunc, this);
}

ThreadWorker::~ThreadWorker()
{
    d_killed = true;
    d_semWork.post();
    d_thread.join();
}

void ThreadWorker::wait()
{
    d_semWait.wait();
}

void ThreadWorker::threadFunc()
{
    while(!d_killed)
    {
        d_semWork.wait();
        if (d_killed)
            break;
        d_func();
        d_semWait.post();
        d_semDone.post();
    }
}
