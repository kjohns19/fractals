#include <fractals/ui/view_manager.hpp>
#include <fractals/ui/application.hpp>
#include <fractals/fractal/fractal.hpp>
#include <cmath>
#include <fstream>
#include <iostream>

namespace frac {

namespace {

void swap_views(std::vector<View>& to, std::vector<View>& from, View& view);

} // close anonymous namespace

ViewManager::ViewManager(FractalWidget& widget, const View& view)
: d_widget(widget)
, d_view(view) {}

const View& ViewManager::getView()
{
    return d_view;
}
void ViewManager::setView(const View& view, int iterations)
{
    d_prev_views.push_back(d_view);
    d_next_views.clear();
    d_view = view;
    resetView(iterations);
}

void ViewManager::nextView()
{
    swap_views(d_prev_views, d_next_views, d_view);
    resetView();
}
void ViewManager::previousView()
{
    swap_views(d_next_views, d_prev_views, d_view);
    resetView();
}

void ViewManager::firstView()
{
    while(!d_prev_views.empty())
        swap_views(d_next_views, d_prev_views, d_view);
    resetView();
}
void ViewManager::lastView()
{
    while(!d_next_views.empty())
        swap_views(d_prev_views, d_next_views, d_view);
    resetView();
}

bool ViewManager::hasNextView() const
{
    return !d_next_views.empty();
}
bool ViewManager::hasPreviousView() const
{
    return !d_prev_views.empty();
}

void ViewManager::zoom(double amount)
{
    View newView(d_view);
    newView.width/=amount;
    newView.height/=amount;
    setView(newView);
}

void ViewManager::zoomIn()
{
    zoom(1.5);
}

void ViewManager::zoomOut()
{
    zoom(1.0/1.5);
}

void ViewManager::resetView(int iterations)
{
    if (iterations == -1)
        iterations = d_widget.fractal().iterations();

    d_widget.fractal().setView(d_view);
    d_widget.fractal().iterate(iterations);
    d_widget.redraw();
    notifyObservers(*this);
}

void ViewManager::clearViews()
{
    d_prev_views.clear();
    d_next_views.clear();
    notifyObservers(*this);
}

namespace {

void swap_views(std::vector<View>& to, std::vector<View>& from, View& view)
{
    if (!from.empty())
    {
        View& newView = from.back();
        to.push_back(view);
        view = newView;
        from.pop_back();
    }
}

} // close anonymous namespace

} // close namespace frac
