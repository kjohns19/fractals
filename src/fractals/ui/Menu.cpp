#include <fractals/color/ColorScheme.hpp>
#include <fractals/fractal/Fractal.hpp>
#include <fractals/ui/Menu.hpp>
#include <fractals/ui/ColorSchemeMenu.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/ui/ViewManager.hpp>
#include <fractals/ui/MenuUtils.hpp>
#include <fractals/fractal/Mandelbrot.hpp>

#include <SFML/Graphics.hpp>

#include <gtkmm.h>
#include <iostream>

void showSaveDialog(Application& app);

struct ViewButtons : public Gtk::ToolButton, public Observer<ViewManager>
{
    ViewButtons(Gtk::Toolbar* toolbar, Application& app):
        vm(app.getViewManager())
    {
        signal_clicked().connect([this]() {
            vm.previousView();
        });
        set_icon_name("gtk-go-back-ltr");
        set_tooltip_text("Previous View");
        toolbar->append(*this);

        nextButton = createToolButton(toolbar, "Next View",
            "gtk-go-forward-ltr", [this]() {
                vm.nextView();
            });

        prevButton = createToolButton(toolbar, "Reset View",
            "gtk-zoom-fit", [this]() {
                vm.firstView();
            });

        createToolButton(toolbar, "Zoom In", "gtk-zoom-in", [this]() {
            vm.zoomIn();
        });

        createToolButton(toolbar, "Zoom Out", "gtk-zoom-out", [this]() {
            vm.zoomOut();
        });

        vm.addObserver(*this);
        notify(vm);
    }

    ~ViewButtons()
    {
        vm.removeObserver(*this);
    }

    void notify(ViewManager& vm)
    {
        set_sensitive(vm.hasPreviousView());
        prevButton->set_sensitive(vm.hasPreviousView());
        nextButton->set_sensitive(vm.hasNextView());
    }
    ViewManager& vm;
    Gtk::ToolButton* prevButton;
    Gtk::ToolButton* nextButton;
};

Gtk::Widget* createMenu(Application& app)
{
    Gtk::Toolbar* menu = Gtk::manage(new Gtk::Toolbar());

    createToolButton(menu, "Save", "gtk-save", [&]() {
        showSaveDialog(app);
    });

    menu->append(*Gtk::manage(new Gtk::SeparatorToolItem()));

    createToolButton(menu, "Reset", "gtk-refresh", [&]() {
        app.getViewManager().resetView();
    });

    Gtk::ToolButton* buttonPlay = createToolButton("Pause", "gtk-media-pause");
    buttonPlay->signal_clicked().connect([buttonPlay, &app]() {
        bool isPlaying = !app.isPaused();
        buttonPlay->set_icon_name(isPlaying ? "gtk-media-play-ltr" : "gtk-media-pause");
        buttonPlay->set_tooltip_text(isPlaying ? "Play" : "Pause");
        app.setPaused(isPlaying);
    });
    menu->append(*buttonPlay);

    menu->append(*Gtk::manage(new Gtk::SeparatorToolItem()));

    Gtk::manage(new ViewButtons(menu, app));

    menu->append(*Gtk::manage(new Gtk::SeparatorToolItem()));

    Gtk::ToggleToolButton* buttonDraw = createToolButton<Gtk::ToggleToolButton>("Fast Drawing", "gtk-edit");
    buttonDraw->set_active(true);
    buttonDraw->signal_clicked().connect([&]() {
        app.getFractal().setDrawMode(!app.getFractal().getDrawMode());
        app.redrawFractal();
    });
    menu->append(*buttonDraw);

    createToolButton(menu, "Path", "gtk-leave-fullscreen", [&]() {
        ViewManager& vm = app.getViewManager();

        ViewManager::View start = vm.getView();
        vm.nextView();
        ViewManager::View end = vm.getView();
        vm.firstView();

        std::vector<ViewManager::View> path = ViewManager::path(start, end, 10);
        for(auto& v: path)
            vm.setView(v);
        vm.firstView();
    });

    createToolButton(menu, "Color Scheme", "gtk-theme-config", [&]() {
        showColorSchemeMenu(app);
    });

    createToolButton(menu, "Step", "gtk-media-forward-ltr", [&]() {
        app.getFractal().iterate();
    });

    menu->show_all();
    return menu;
}

void showSaveDialog(Application& app)
{
    Gtk::Dialog dialog("Save Image", app.getWindow(), true);
    const sf::Vector2u& wsize = app.getWindowSize();

    Gtk::Box* content;
    Gtk::Box* row;
    Gtk::SpinButton* entryWidth;
    Gtk::SpinButton* entryHeight;
    Gtk::SpinButton* entryIterations;
    Gtk::Entry* entryFile;
    Gtk::Label* label;

    Gtk::Button* saveButton = dialog.add_button("Save", Gtk::RESPONSE_ACCEPT);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

    saveButton->set_sensitive(false);


    content = dialog.get_content_area();

    //Dimensions
    row = Gtk::manage(new Gtk::HBox());
    row->pack_start(*Gtk::manage(new Gtk::Label("Width: ", Gtk::ALIGN_END)));

    entryWidth = Gtk::manage(new Gtk::SpinButton());
    entryWidth->set_range(1, 99999);
    entryWidth->set_increments(1, 10);
    entryWidth->set_value(wsize.x);

    entryHeight = Gtk::manage(new Gtk::SpinButton());
    entryHeight->set_range(1, 99999);
    entryHeight->set_increments(1, 10);
    entryHeight->set_value(wsize.y);

    row->pack_start(*entryWidth);
    row->pack_start(*Gtk::manage(new Gtk::Label("Height: ", Gtk::ALIGN_END)));
    row->pack_start(*entryHeight);
    content->pack_start(*row);


    //Iterations
    row = Gtk::manage(new Gtk::HBox());
    row->pack_start(*Gtk::manage(new Gtk::Label("Iterations: ", Gtk::ALIGN_END)));
    entryIterations = Gtk::manage(new Gtk::SpinButton());
    entryIterations->set_range(1, 99999);
    entryIterations->set_increments(1, 10);
    entryIterations->set_value(app.getFractal().iterations());
    row->pack_start(*entryIterations);
    content->pack_start(*row);


    //Filename
    row = Gtk::manage(new Gtk::HBox());
    entryFile = Gtk::manage(new Gtk::Entry());
    entryFile->set_editable(false);
    label = Gtk::manage(new Gtk::Label("File:", Gtk::ALIGN_END));
    row->pack_start(*label);
    row->pack_start(*entryFile);
    createButton(row, "Select", "gtk-save", [&]() {
        Gtk::FileChooserDialog fc(app.getWindow(), "Select file", Gtk::FILE_CHOOSER_ACTION_SAVE);
        fc.add_button("Cancel", Gtk::RESPONSE_CANCEL);
        fc.add_button("Select", Gtk::RESPONSE_ACCEPT);

        int result = fc.run();
        std::cout << "Result: " << result << std::endl;
        if (result == Gtk::RESPONSE_ACCEPT)
        {
            entryFile->set_text(fc.get_filename());
            saveButton->set_sensitive(!fc.get_filename().empty());
        }
    });
    content->pack_start(*row);

    dialog.show_all();
    int result = dialog.run();

    std::cout << "Result: " << result << std::endl;
    if (result == Gtk::RESPONSE_ACCEPT)
    {
        sf::Vector2u size(entryWidth->get_value_as_int(),
                          entryHeight->get_value_as_int());

        std::cout << "Creating fractal..." << std::endl;
        Mandelbrot fractal(size, app.getViewManager().getView());
        fractal.iterate(entryIterations->get_value_as_int());
        fractal.setDrawMode(false);

        std::cout << "Creating image..." << std::endl;
        sf::RenderTexture texture;
        texture.create(size.x, size.y);

        fractal.draw(texture, app.getColorScheme());
        texture.display();

        sf::Image image = texture.getTexture().copyToImage();

        std::cout << "Saving image..." << std::endl;
        image.saveToFile(entryFile->get_text());
        std::cout << "Done!" << std::endl;
    }
}
