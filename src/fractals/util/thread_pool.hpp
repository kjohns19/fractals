#ifndef INCLUDED_FRAC_THREAD_POOL_HPP
#define INCLUDED_FRAC_THREAD_POOL_HPP

#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace frac {

class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(unsigned numThreads);
    ~ThreadPool();

    template<typename Func, typename... Args>
    std::future<std::result_of_t<Func(Args...)>> apply(Func f, Args&&... args);

    template<typename Func, typename InputIt, typename OutputIt>
    void map(InputIt begin, InputIt end, OutputIt outBegin, Func func);

    template<typename Func, typename InputIt>
    void map(InputIt begin, InputIt end, Func func);
private:
    class Task
    {
    public:
        virtual void doWork() = 0;
    };

    template<typename Func>
    class FuncTask : public Task
    {
    private:
        using RT = std::result_of_t<Func&&()>;
    public:
        FuncTask(Func function): d_task(function) {}

        void doWork() override { d_task(); }

        std::future<RT> get_future() { return d_task.get_future(); }
    private:
        std::packaged_task<RT()> d_task;
    };

    void workerThread();

    std::vector<std::thread> d_threads;

    std::mutex d_mutex;

    // Protected by mutex
    std::deque<std::unique_ptr<Task>> d_tasks;
    std::condition_variable d_condition;
    bool d_exit;
};

template<typename Func, typename... Args>
std::future<std::result_of_t<Func(Args...)>> ThreadPool::apply(
        Func f, Args&&... args)
{
    // TODO perfect capture
    auto lambda = [=]() {
        return f(args...);
    };

    auto task = std::make_unique<FuncTask<decltype(lambda)>>(lambda);

    auto future = task->get_future();

    std::lock_guard<std::mutex> lock(d_mutex);
    d_tasks.push_back(std::move(task));
    d_condition.notify_one();
    return std::move(future);
}

template<typename Func, typename InputIt, typename OutputIt>
void ThreadPool::map(
        InputIt begin, InputIt end, OutputIt outBegin, Func func)
{
    using ItVal = typename std::iterator_traits<InputIt>::value_type;
    using RT = std::result_of_t<Func(ItVal)>;
    std::vector<std::future<RT>> futures;
    for(auto it = begin; it != end; ++it)
        futures.emplace_back(apply(func, *it));

    for(auto& future: futures)
    {
        *outBegin = std::move(future.get());
        ++outBegin;
    }
}

template<typename Func, typename InputIt>
void ThreadPool::map(InputIt begin, InputIt end, Func func)
{
    using ItVal = typename std::iterator_traits<InputIt>::value_type;
    using RT = std::result_of_t<Func(ItVal)>;
    std::vector<std::future<RT>> futures;
    for(auto it = begin; it != end; ++it)
        futures.emplace_back(apply(func, *it));

    for(auto& future: futures)
        future.get();
}

} // close namespace frac

#endif // INCLUDED_FRAC_THREAD_POOL_HPP
