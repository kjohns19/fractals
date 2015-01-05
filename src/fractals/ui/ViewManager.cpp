#include <fractals/ui/ViewManager.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/fractal/Fractal.hpp>
#include <cmath>

ViewManager::ViewManager(Application& app, const View& view): d_app(app),
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

void ViewManager::zoom(double amount)
{
    View newView(d_view);
    newView.width/=amount;
    newView.height/=amount;
    newView.left = d_view.left + (d_view.width - newView.width) / 2;
    newView.top = d_view.top + (d_view.height - newView.height) / 2;
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

void ViewManager::resetView()
{
    d_app.getFractal().setView(d_view);
    d_app.getFractal().iterate(500);
    d_app.redrawFractal();
    notify(*this);
}

void ViewManager::clearViews()
{
    d_prev_views.clear();
    d_next_views.clear();
    notify(*this);
}

std::vector<ViewManager::View> ViewManager::path(const View& start, const View& end, int count)
{
    sf::Vector2<double> startCenter(start.left + start.width/2,
                                    start.top + start.height/2);
    sf::Vector2<double> endCenter(end.left + end.width/2, 
                                  end.top + end.height/2);

    sf::Vector2<double> startSize(start.width, start.height);
    sf::Vector2<double> endSize(end.width, end.height);

    double sizeScale = end.width / start.width;

    sf::Vector2<double> diffCenter = endCenter - startCenter;

    std::vector<View> path;

    double ratio = std::pow(sizeScale, 1.0/(count-1));
    double sum = (std::pow(ratio, count) - 1) / (ratio - 1);

    for(int i = 0; i < count; i++)
    {
        double scale = 1.0 * i / (count - 1);

        double amount = (std::pow(ratio, i+1) - 1) / (ratio - 1);
        double offX = diffCenter.x / sum * amount;
        double offY = diffCenter.y / sum * amount;
        sf::Vector2<double> center = startCenter + sf::Vector2<double>(offX, offY);//diffCenter * scale;
        sf::Vector2<double> size = startSize * std::pow(sizeScale, scale);

        path.push_back(View(center.x - size.x/2,
                            center.y - size.y/2,
                            size.x, size.y));
    }

    return path;
}
