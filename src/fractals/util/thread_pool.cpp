#include <fractals/util/thread_pool.hpp>

namespace frac {

ThreadPool::ThreadPool()
: ThreadPool(std::thread::hardware_concurrency()) {}

ThreadPool::ThreadPool(unsigned numThreads)
: d_exit(false)
{
    auto threadFunc = [this]() { workerThread(); };
    for (unsigned i = 0; i < numThreads; i++)
        d_threads.emplace_back(threadFunc);
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(d_mutex);
        d_exit = true;
        d_condition.notify_all();
    }
    for(auto& thread: d_threads)
        thread.join();
}

void ThreadPool::workerThread()
{
    while(1)
    {
        std::unique_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lock(d_mutex);
            d_condition.wait(lock, [this](){
                return d_exit || !d_tasks.empty();
            });
            if (d_tasks.empty())
            {
                if (d_exit)
                    return;
            }
            else
            {
                task = std::move(d_tasks.front());
                d_tasks.pop_front();
            }
        }
        if (task)
            task->doWork();
    }
}

} // close namespace frac
