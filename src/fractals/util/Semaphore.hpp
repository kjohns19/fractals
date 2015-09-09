#ifndef INCLUDED_SEMAPHORE_HPP
#define INCLUDED_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
    Semaphore(unsigned start = 0);
    void post();
    void wait();
    bool tryWait();
private:
    std::mutex d_mutex;
    std::condition_variable d_cv;
    unsigned d_count;
};

#endif //INCLUDED_SEMAPHORE_HPP
