#include <fractals/ui/view_manager.hpp>

#include <fractals/fractal/fractal.hpp>
#include <fractals/ui/application.hpp>

#include <cmath>
#include <fstream>
#include <iostream>

namespace frac {

ViewManager::ViewManager(FractalWidget& widget)
: d_widget(widget) {}

const View& ViewManager::getView()
{
    return d_widget.fractal().getView();
}
void ViewManager::setView(const View& view, int iterations)
{
    d_prev_views.push_back(getView());
    d_next_views.clear();
    resetView(view, iterations);
}

void ViewManager::nextView()
{
    swapViews(d_prev_views, d_next_views);
}
void ViewManager::previousView()
{
    swapViews(d_next_views, d_prev_views);
}

void ViewManager::firstView()
{
    swapAll(d_next_views, d_prev_views);
}
void ViewManager::lastView()
{
    swapAll(d_prev_views, d_next_views);
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
    View newView(getView());
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

void ViewManager::resetView(const View& view, int iterations)
{
    auto& fractal = d_widget.fractal();

    if (iterations == -1)
        iterations = fractal.iterations();

    fractal.setView(view);
    fractal.iterate(iterations);
    d_widget.redraw();
    notifyObservers(*this);
}

void ViewManager::clearViews()
{
    d_prev_views.clear();
    d_next_views.clear();
    notifyObservers(*this);
}

void ViewManager::swapViews(std::vector<View>& to, std::vector<View>& from)
{
    if (from.empty())
        return;

    to.push_back(getView());
    View newView = from.back();
    from.pop_back();

    resetView(newView);
}

void ViewManager::swapAll(std::vector<View>& to, std::vector<View>& from)
{
    to.push_back(getView());
    to.insert(to.end(), from.rbegin(), from.rend());
    View view = to.back();
    to.pop_back();

    from.clear();

    resetView(view);
}

} // close namespace frac
