#ifndef INCLUDED_VIEW_MANAGER_HPP
#define INCLUDED_VIEW_MANAGER_HPP

#include <fractals/util/Subject.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <vector>

class Application;

class ViewManager : public Subject<ViewManager>
{
public:
    typedef sf::Rect<double> View;

    static std::vector<View> path(const View& start, const View& end, int count);

    ViewManager(Application& app, const View& view);

    const View& getView();
    void setView(const View& view);
    void resetView();
    void nextView();
    void previousView();
    void firstView();
    void lastView();
    void zoom(double amount);
    void zoomIn();
    void zoomOut();

    void clearViews();

    bool hasNextView() const;
    bool hasPreviousView() const;
private:
    Application& d_app;

    View d_view;
    std::vector<View> d_prev_views;
    std::vector<View> d_next_views;
};

#endif //INCLUDED_VIEW_MANAGER_HPP
