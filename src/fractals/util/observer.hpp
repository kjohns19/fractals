#ifndef INCLUDED_FRAC_OBSERVER_HPP
#define INCLUDED_FRAC_OBSERVER_HPP

namespace frac {

template<typename T>
class Observer
{
public:
    virtual void update(T object) = 0;
private:
};

} // close namespace frac

#endif //INCLUDED_FRAC_OBSERVER_HPP
