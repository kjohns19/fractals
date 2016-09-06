#ifndef INCLUDED_FRAC_VIEW_MANAGER_HPP
#define INCLUDED_FRAC_VIEW_MANAGER_HPP

#include <fractals/util/view.hpp>

#include <kj/subject.hpp>

#include <SFML/Graphics/Rect.hpp>

#include <map>
#include <vector>

namespace frac {

class Application;

class ViewManager : public kj::Subject<ViewManager&>
{
public:
    ViewManager(Application& app, const View& view);
    ~ViewManager();

    const View& getView();
    void setView(const View& view, int iterations = -1);
    void resetView(int iterations = -1);
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

    std::map<std::string, std::pair<View, int> >& getSavedViews();
    void saveViews() const;
private:
    Application& d_app;

    View d_view;
    std::vector<View> d_prev_views;
    std::vector<View> d_next_views;

    std::map<std::string, std::pair<View, int> > d_saved_views;
    bool d_loaded;
};

} // close namespace frac

#endif //INCLUDED_FRAC_VIEW_MANAGER_HPP
