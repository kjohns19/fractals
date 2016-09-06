#ifndef INCLUDED_FRAC_EVENT_HANDLER
#define INCLUDED_FRAC_EVENT_HANDLER

#include <memory>

namespace sf {

class Event;

} // close namespace sf

namespace frac {

class EventHandler
{
public:
    EventHandler(std::shared_ptr<EventHandler> next);

    virtual ~EventHandler() {}

    void handle(const sf::Event& event);
private:
    virtual bool doHandle(const sf::Event& event) = 0;

    std::shared_ptr<EventHandler> d_next;
};

} // close namespace frac

#endif //INCLUDED_FRAC_EVENT_HANDLER
