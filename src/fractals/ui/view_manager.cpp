#include <fractals/ui/view_manager.hpp>
#include <fractals/ui/application.hpp>
#include <fractals/fractal/fractal.hpp>
#include <cmath>
#include <fstream>
#include <iostream>

namespace {

void swap_views(std::vector<View>& to, std::vector<View>& from, View& view);

} // close anonymous namespace

ViewManager::ViewManager(Application& app, const View& view):
    d_app(app),
    d_view(view),
    d_loaded(false) {}

ViewManager::~ViewManager()
{
    saveViews();
}

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
        iterations = d_app.getFractal().iterations();

    d_app.getFractal().setView(d_view);
    d_app.getFractal().iterate(iterations);
    d_app.redrawFractal();
    notifyObservers(*this);
}

void ViewManager::clearViews()
{
    d_prev_views.clear();
    d_next_views.clear();
    notifyObservers(*this);
}

std::map<std::string, std::pair<View, int> >& ViewManager::getSavedViews()
{
    if (!d_loaded)
    {
        d_loaded = true;
        std::ifstream in("views.dat");
        if (in)
        {
            size_t count;
            size_t nameLength;
            std::string name;
            View view;
            int iterations;
            in.read(reinterpret_cast<char*>(&count), sizeof(count));
            for(size_t i = 0; i < count; i++)
            {
                in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                name.resize(nameLength, ' ');

                in.read(&*name.begin(), nameLength);
                in.read(reinterpret_cast<char*>(&view.x),   sizeof(view.x));
                in.read(reinterpret_cast<char*>(&view.y),    sizeof(view.y));
                in.read(reinterpret_cast<char*>(&view.width),  sizeof(view.width));
                in.read(reinterpret_cast<char*>(&view.height), sizeof(view.height));
                in.read(reinterpret_cast<char*>(&iterations), sizeof(int));

                d_saved_views[name] = std::make_pair(view, iterations);
            }
        }
    }
    return d_saved_views;
}

void ViewManager::saveViews() const
{
    if (d_loaded)
    {
        std::ofstream out("views.dat");
        if (out)
        {
            size_t size = d_saved_views.size();
            out.write(reinterpret_cast<char*>(&size), sizeof(size));
            for(auto& pair: d_saved_views)
            {
                const std::string& name = pair.first;
                size_t nameLength = name.length();
                out.write(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                out.write(name.c_str(), nameLength);

                const View view = pair.second.first;
                int iterations = pair.second.second;

                out.write(reinterpret_cast<const char*>(&view.x),   sizeof(view.x));
                out.write(reinterpret_cast<const char*>(&view.y),    sizeof(view.y));
                out.write(reinterpret_cast<const char*>(&view.width),  sizeof(view.width));
                out.write(reinterpret_cast<const char*>(&view.height), sizeof(view.height));
                out.write(reinterpret_cast<const char*>(&iterations), sizeof(int));
            }
        }
    }
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
