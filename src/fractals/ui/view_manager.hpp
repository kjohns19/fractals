#ifndef INCLUDED_FRAC_VIEW_MANAGER_HPP
#define INCLUDED_FRAC_VIEW_MANAGER_HPP

#include <fractals/util/subject.hpp>
#include <fractals/util/view.hpp>

#include <SFML/Graphics/Rect.hpp>

#include <map>
#include <vector>

namespace frac {

class FractalWidget;

class ViewManager : public Subject<ViewManager&>
{
public:
    ViewManager(FractalWidget& widget);

    const View& getView();
    void setView(const View& view, int iterations = -1);
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
    void resetView(const View& view, int iterations = -1);
    void swapViews(std::vector<View>& to, std::vector<View>& from);
    void swapAll(std::vector<View>& to, std::vector<View>& from);

    FractalWidget& d_widget;

    std::vector<View> d_prev_views;
    std::vector<View> d_next_views;
};

} // close namespace frac

#endif //INCLUDED_FRAC_VIEW_MANAGER_HPP
