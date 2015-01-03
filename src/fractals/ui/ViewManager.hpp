#ifndef INCLUDED_VIEW_MANAGER_HPP
#define INCLUDED_VIEW_MANAGER_HPP

#include <fractals/util/Subject.hpp>

#include <SFML/Graphics/Rect.hpp>

class Application;

class ViewManager : public Subject<ViewManager>
{
public:
    typedef sf::Rect<double> View;

    ViewManager(Application& app, const View& view);

    const View& getView();
    void setView(const View& view);
    void resetView();
    void nextView();
    void previousView();
    void firstView();
    void lastView();

    bool hasNextView() const;
    bool hasPreviousView() const;
private:
    Application& d_app;

    View d_view;
    std::vector<View> d_prev_views;
    std::vector<View> d_next_views;
};

#endif //INCLUDED_VIEW_MANAGER_HPP
