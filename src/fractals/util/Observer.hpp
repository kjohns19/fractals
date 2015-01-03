#ifndef INCLUDED_OBSERVER_HPP
#define INCLUDED_OBSERVER_HPP

template<typename T, typename... Args>
class Observer
{
public:
    virtual ~Observer() {}
    virtual void notify(T& object, Args...) = 0;
};

#endif //INCLUDED_OBSERVER_HPP
