#include <fractals/ui/application.hpp>

#include <fractals/color/color_scheme.hpp>
#include <fractals/fractal/fractal.hpp>
#include <fractals/ui/color_scheme_editor.hpp>
#include <fractals/ui/dialog_fractal.hpp>
#include <fractals/ui/dialog_load_save.hpp>
#include <fractals/ui/dialog_save_image.hpp>
#include <fractals/ui/view_changer.hpp>
#include <fractals/util/builder_utils.hpp>
#include <fractals/util/raii_helpers.hpp>
#include <fractals/util/sfml_widget.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/dialog.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/toggletoolbutton.h>

#include <cstdlib>
#include <iostream>

namespace frac {
namespace {

// Controls the sensitivity of the view toolbar buttons
struct ViewObserver: Observer<ViewManager&>
{
    ViewObserver(
        Gtk::ToolButton& viewDefault,
        Gtk::ToolButton& viewPrevious,
        Gtk::ToolButton& viewNext)
    : d_viewDefault(viewDefault)
    , d_viewPrevious(viewPrevious)
    , d_viewNext(viewNext) {}

    void update(ViewManager& vm)
    {
        d_viewDefault.set_sensitive(vm.hasPreviousView());
        d_viewPrevious.set_sensitive(vm.hasPreviousView());
        d_viewNext.set_sensitive(vm.hasNextView());
    }

    Gtk::ToolButton& d_viewDefault;
    Gtk::ToolButton& d_viewPrevious;
    Gtk::ToolButton& d_viewNext;
};

};

Application::Application(
        const sf::Vector2u& windowSize,
        std::unique_ptr<Fractal> fractal,
        ColorScheme colorScheme)
: d_windowSize(windowSize)
, d_fractalWidget(Gtk::manage(
    new FractalWidget({windowSize.x, windowSize.y}, std::move(fractal))))
, d_viewManager(*d_fractalWidget)
, d_window(nullptr)
{
    using BU = BuilderUtils;

    d_fractalWidget->colorScheme() = colorScheme;
    d_fractalWidget->show();

    auto builder = Gtk::Builder::create();
    try
    {
        builder->add_from_file("data/ui.glade");
    }
    catch (Gtk::BuilderError& e)
    {
        std::cerr << e.code() << ": " << e.what()  << std::endl;
        std::exit(1);
    }

    d_window = &BU::getWidget<Gtk::Window>(
        builder, "window-main");

    auto& fractalArea = BU::getWidget<Gtk::Box>(
        builder, "fractal-area");
    fractalArea.pack_start(*d_fractalWidget, Gtk::PACK_SHRINK);

    auto viewChanger = std::make_shared<ViewChanger>(*this, nullptr);
    d_fractalWidget->eventHandler(viewChanger);

    d_fractalWidget->onDraw([this, viewChanger] (FractalWidget& widget) {
        sf::RenderTarget& target = widget.window();
        viewChanger->drawBounds(target);
    });

    initMenu(builder);
    initToolbar(builder);
}

void Application::initMenu(Glib::RefPtr<Gtk::Builder> builder)
{
    using BU = BuilderUtils;

    DialogLoadSave::configure(
        *this, builder, "dialog-load", "dialog-save", "file-load", "file-save");
    DialogSaveImage::configure(*this, builder, "dialog-save-image", "file-save-image");

    BU::getWidget<Gtk::ImageMenuItem>(builder, "file-quit")
    .signal_activate().connect([this]() {
        if (d_app)
            d_app->quit();
    });

    auto& fastDrawButton = BU::getWidget<Gtk::CheckMenuItem>(
        builder, "view-fastdraw");
    auto fastDrawButtonSignal = [this, &fastDrawButton]() {
        d_fractalWidget->fractal().setDrawMode(fastDrawButton.get_active());
        d_fractalWidget->redraw();
    };
    fastDrawButton.signal_toggled().connect(fastDrawButtonSignal);
}

void Application::initToolbar(Glib::RefPtr<Gtk::Builder> builder)
{
    using BU = BuilderUtils;

    // Play/Pause
    auto& playButton = BU::getWidget<Gtk::ToggleToolButton>(
        builder, "tool-play");
    auto playButtonSignal = [this, &playButton]() {
        bool running = playButton.get_active();
        playButton.set_icon_name(
            running ? "media-playback-pause" : "media-playback-start");
        fractalWidget().setRunning(running);
    };
    playButton.signal_clicked().connect(playButtonSignal);
    playButtonSignal();

    // Step
    BU::getWidget<Gtk::ToolButton>(builder, "tool-step")
    .signal_clicked().connect([this]() {
        fractal().iterate();
    });

    // Jump
    auto& jumpDialog = BU::getWidget<Gtk::Dialog>(builder, "dialog-jump");
    auto& jumpSpinner = BU::getWidget<Gtk::SpinButton>(
        builder, "dialog-jump-spinner");
    BU::getWidget<Gtk::ToolButton>(builder, "tool-jump")
    .signal_clicked().connect([this, &jumpDialog, &jumpSpinner]() {
        PauseFractal pause(fractalWidget());

        int current = fractal().iterations();
        jumpSpinner.set_value(current);

        int result = jumpDialog.run();
        jumpDialog.hide();

        if (result == Gtk::RESPONSE_OK)
        {
            int target = jumpSpinner.get_value_as_int();
            if (target > current)
                fractal().iterate(target-current);
            else
            {
                fractal().reset();
                fractal().iterate(target);
            }
            fractalWidget().redraw();
        }
    });

    // Views - default, previous, next, zoom in, zoom out
    auto& viewDefault = BU::getWidget<Gtk::ToolButton>(
        builder, "tool-view-default");
    auto& viewPrevious = BU::getWidget<Gtk::ToolButton>(
        builder, "tool-view-prev");
    auto& viewNext = BU::getWidget<Gtk::ToolButton>(
        builder, "tool-view-next");

    viewDefault.signal_clicked().connect([this]() {
        d_viewManager.firstView();
    });
    viewPrevious.signal_clicked().connect([this]() {
        d_viewManager.previousView();
    });
    viewNext.signal_clicked().connect([this]() {
        d_viewManager.nextView();
    });

    d_viewObserver = std::make_unique<ViewObserver>(
        viewDefault, viewPrevious, viewNext);
    d_viewManager.addObserver(*d_viewObserver);

    BU::getWidget<Gtk::ToolButton>(builder, "tool-view-zoom-in")
    .signal_clicked().connect([this]() {
        d_viewManager.zoomIn();
    });
    BU::getWidget<Gtk::ToolButton>(builder, "tool-view-zoom-out")
    .signal_clicked().connect([this]() {
        d_viewManager.zoomOut();
    });

    // Colorscheme
    ColorSchemeEditor::configure(
        *this, builder, "dialog-colorscheme", "tool-colorscheme");

    DialogFractal::configure(
        *this, builder, "dialog-fractal", "tool-fractal");
}

void Application::run(Glib::RefPtr<Gtk::Application> app)
{
    d_app = app;
    app->run(*d_window);
}

} // close namespace frac
