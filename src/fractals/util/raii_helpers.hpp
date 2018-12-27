#ifndef INCLUDED_FRAC_RAII_HELPERS_HPP
#define INCLUDED_FRAC_RAII_HELPERS_HPP

#include <fractals/ui/fractal_widget.hpp>

namespace frac {

// Helper RAII class to pause a fractal widget from iterating
class PauseFractal
{
public:
    PauseFractal(FractalWidget& widget)
    : d_widget(widget)
    , d_running(widget.isRunning()) { d_widget.setRunning(false); }

    ~PauseFractal() { d_widget.setRunning(d_running); }
private:
    FractalWidget& d_widget;
    bool d_running;
};

} // close namespace frac

#endif // INCLUDED_FRAC_RAII_HELPERS_HPP
