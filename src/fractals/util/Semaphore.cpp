#include <fractals/util/Semaphore.hpp>
#include <iostream>

Semaphore::Semaphore(unsigned start)
: d_count(start) {}

void Semaphore::post()
{
    std::lock_guard<std::mutex> lock(d_mutex);
    d_count++;
    d_cv.notify_one();
}

void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(d_mutex);
    d_cv.wait(lock, [this] { return d_count != 0; });
    d_count--;
}

bool Semaphore::tryWait()
{
    std::unique_lock<std::mutex> lock(d_mutex);
    if (d_count == 0)
        return false;
    d_count--;
    return true;
}
