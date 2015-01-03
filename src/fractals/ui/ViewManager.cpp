#include <fractals/ui/ViewManager.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/fractal/Fractal.hpp>

ViewManager::ViewManager(Application& app, const View& view):
    d_app(app),
    d_view(view) {}

const ViewManager::View& ViewManager::getView()
{
    return d_view;
}
void ViewManager::setView(const View& view)
{
    d_prev_views.push_back(d_view);
    d_next_views.clear();
    d_view = view;
    resetView();
}

static void swap_views(std::vector<ViewManager::View>& to,
                       std::vector<ViewManager::View>& from,
                       ViewManager::View& view)
{
    if (!from.empty())
    {
        ViewManager::View& newView = from.back();
        to.push_back(view);
        view = newView;
        from.pop_back();
    }
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

void ViewManager::resetView()
{
    d_app.getFractal().setView(d_view);
    d_app.getFractal().iterate(500);
    notify(*this);
}
