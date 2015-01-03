#ifndef INCLUDED_SUBJECT_HPP
#define INCLUDED_SUBJECT_HPP

#include <fractals/util/Observer.hpp>

#include <set>

template<typename T, typename... Args>
class Subject
{
public:
    void addObserver(Observer<T, Args...>& observer);
    void removeObserver(Observer<T, Args...>& observer);
protected:
    void notify(T& object, Args... args);
private:
    std::set<Observer<T, Args...>*> d_observers;
};

template<typename T, typename... Args>
void Subject<T, Args...>::addObserver(Observer<T, Args...>& observer)
{
    d_observers.insert(&observer);
}

template<typename T, typename... Args>
void Subject<T, Args...>::removeObserver(Observer<T, Args...>& observer)
{
    d_observers.erase(&observer);
}

template<typename T, typename... Args>
void Subject<T, Args...>::notify(T& object, Args... args)
{
    for(auto observer: d_observers)
        observer->notify(object, args...);
}

#endif //INCLUDED_SUBJECT_HPP
