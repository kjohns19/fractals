#include <fractals/color/color_scheme.hpp>
#include <fractals/fractal/fractal.hpp>
#include <fractals/ui/menu.hpp>
#include <fractals/util/view.hpp>
#include <fractals/ui/color_scheme_menu.hpp>
#include <fractals/ui/application.hpp>
#include <fractals/ui/view_manager.hpp>
#include <fractals/ui/menu_utils.hpp>
#include <fractals/fractal/mandelbrot.hpp>
#include <fractals/fractal/julia.hpp>
#include <kj/sfml_widget.hpp>
#include <kj/observer.hpp>

#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/separatortoolitem.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/widget.h>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <iomanip>

namespace {

void showSaveDialog(Application& app);
void showIterateDialog(Application& app);
void showViewDialog(Application& app);
void showNewDialog(Application& app);
void showSetViewDialog(Application& app);

void save(const std::string& filename, Fractal& fractal, const ColorScheme& cs, const sf::Vector2u& size, const View& view, int iterations);

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T>& vec)
{
    return out << "(" << vec.x << ", " << vec.y << ")";
}

} // close anonymous namespace


struct ViewButtons : public Gtk::ToolButton, public kj::Observer<ViewManager&>
{
    ViewButtons(Gtk::Toolbar* toolbar, Application& app)
    : vm(app.getViewManager())
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

        firstButton = createToolButton(toolbar, "Reset View",
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
        update(vm);
    }

    ~ViewButtons()
    {
        vm.removeObserver(*this);
    }

    void update(ViewManager& vm)
    {
        set_sensitive(vm.hasPreviousView());
        firstButton->set_sensitive(vm.hasPreviousView());
        nextButton->set_sensitive(vm.hasNextView());
    }
    ViewManager& vm;
    Gtk::ToolButton* firstButton;
    Gtk::ToolButton* nextButton;
};

Gtk::Widget* createMenu(Application& app)
{
    Gtk::Toolbar* menu = Gtk::manage(new Gtk::Toolbar());

    createToolButton(menu, "Save", "gtk-save", [&]() {
        showSaveDialog(app);
    });

    Gtk::SeparatorToolItem* separator = Gtk::manage(new Gtk::SeparatorToolItem());
    separator->set_draw(true);
    menu->append(*separator);

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

    separator = Gtk::manage(new Gtk::SeparatorToolItem());
    separator->set_draw(true);
    menu->append(*separator);

    Gtk::manage(new ViewButtons(menu, app));

    separator = Gtk::manage(new Gtk::SeparatorToolItem());
    separator->set_draw(true);
    menu->append(*separator);

    Gtk::ToggleToolButton* buttonDraw = createToolButton<Gtk::ToggleToolButton>("Fast Drawing", "gtk-edit");
    buttonDraw->set_active(true);
    buttonDraw->signal_clicked().connect([&]() {
        app.getFractal().setDrawMode(!app.getFractal().getDrawMode());
        app.redrawFractal();
    });
    menu->append(*buttonDraw);

    createToolButton(menu, "Path", "gtk-leave-fullscreen", [&]() {
        //Gtk::Dialog dialog("Save Path", app.getWindow(), true);

        ViewManager& vm = app.getViewManager();
        int iterations = app.getFractal().iterations();

        View start = vm.getView();
        vm.nextView();
        View end = vm.getView();
        vm.firstView();

        std::vector<View> path = start.path(end, 1000);

        for(auto& v: path)
            std::cout << "View: " << v << "   Ratio: " << (v.width/v.height) << std::endl;
        int i = 1;
        for(auto& v: path)
        {
            std::cout << i << ":" << std::endl;
            std::stringstream ss;
            ss << "data/zoom/" << (i++) << ".jpg";
            save(ss.str(), app.getFractal(), app.getColorScheme(), app.getWindowSize(), v, iterations);
        }
    });

    createToolButton(menu, "Color Scheme", "gtk-theme-config", [&]() {
        showColorSchemeMenu(app);
    });

    createToolButton(menu, "Step", "gtk-media-forward-ltr", [&]() {
        app.getFractal().iterate();
    });

    createToolButton(menu, "Jump To Iteration", "gtk-jump-to-ltr", [&]() {
        showIterateDialog(app);
    });

    createToolButton(menu, "Views", "gtk-find-and-replace", [&]() {
        showViewDialog(app);
    });

    createToolButton(menu, "View", "gtk-find", [&]() {
        View view = app.getViewManager().getView();
        std::cout << std::setprecision(30) << std::scientific;
        std::cout << "X: " << view.x << std::endl;
        std::cout << "Y: " << view.y << std::endl;
        std::cout << "W: " << view.width << std::endl;
        std::cout << "H: " << view.height << std::endl;
    });


    createToolButton(menu, "New", "gtk-new", [&]() {
        showNewDialog(app);
    });

    createToolButton(menu, "Set View", "gtk-find", [&]() {
        showSetViewDialog(app);
    });

    createToolButton(menu, "Test", "gtk-new", [&]() {
        static int count = 0;
        double ax = -0.4;
        double ay = 0.6;
        double bx = 0.285;
        double by = 0.01;
        int total = 50;

        count++;

        double cx = bx + (ax - bx) * count / total;
        double cy = by + (ay - by) * count / total;
        std::cout << cx << "   " << cy << std::endl;

        Julia* julia = dynamic_cast<Julia*>(&app.getFractal());
        if (julia)
        {
            julia->setValue(cx, cy);
            app.redrawFractal();
        }
    });

    menu->show_all();
    return menu;
}

namespace {

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

        
        save(entryFile->get_text(),
             app.getFractal(),
             app.getColorScheme(),
             size,
             app.getViewManager().getView(),
             entryIterations->get_value_as_int());
    }
}

void showIterateDialog(Application& app)
{
    Gtk::Dialog dialog("Jump To Iteration", app.getWindow(), true);

    dialog.add_button("Ok", Gtk::RESPONSE_ACCEPT);

    Gtk::Box* content = dialog.get_content_area();

    Fractal& fractal = app.getFractal();

    int currentIterations = fractal.iterations();
    Gtk::SpinButton iterationEntry;
    iterationEntry.set_range(0, 99999);
    iterationEntry.set_increments(1, 10);
    iterationEntry.set_value(currentIterations);

    content->pack_start(iterationEntry);
    content->show_all();

    int result = dialog.run();
    if (result == Gtk::RESPONSE_ACCEPT)
    {
        int val = iterationEntry.get_value_as_int();
        if (val > currentIterations)
            fractal.iterate(val - currentIterations);
        else if (val < currentIterations)
        {
            fractal.setView(app.getViewManager().getView());
            fractal.iterate(val);
        }
        if (val != currentIterations)
            app.redrawFractal();
    }
}

typedef std::pair<View, int> ViewPair;

void addViewRow(Gtk::ListBox& list, const std::string& name);

int listSortFunc(Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2)
{
    Gtk::Label* label1 = dynamic_cast<Gtk::Label*>(row1->get_child());
    Gtk::Label* label2 = dynamic_cast<Gtk::Label*>(row2->get_child());

    return label1->get_text() < label2->get_text();
}

void showViewDialog(Application& app)
{

    Gtk::Dialog dialog("View List", app.getWindow(), true);

    dialog.add_button("Ok", Gtk::RESPONSE_ACCEPT);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

    Gtk::Box* content = dialog.get_content_area();

    ViewManager& vm = app.getViewManager();
    Fractal& fractal = app.getFractal();

    std::map<std::string, ViewPair>& views = vm.getSavedViews();
    const std::string* viewName = nullptr;
    ViewPair* viewView = nullptr;

    Gtk::ListBox list;
    list.set_sort_func(sigc::ptr_fun(listSortFunc));

    const int previewWidth = 300;
    const int previewHeight = 200;
    kj::SFMLWidget previewWidget(sf::VideoMode(previewWidth, previewHeight));

    Gtk::HBox entryBox;
    Gtk::Entry entryName;
    entryBox.pack_start(entryName);
    createButton(&entryBox, "Add", "gtk-add", [&]() {
        const std::string& name = entryName.get_text();
        const View& view = vm.getView();
        int iteration = fractal.iterations();
        views[name] = ViewPair(view, iteration);
        addViewRow(list, name);
        vm.saveViews();
    });
    Gtk::Button* removeButton = createButton(&entryBox, "Remove", "gtk-remove", [&]() {
        Gtk::ListBoxRow* row = list.get_selected_row();
        if (row)
        {
            views.erase(*viewName);
            list.remove(*row);
            viewName = nullptr;
            viewView = nullptr;
        }
    });
    removeButton->set_sensitive(false);

    auto connection = list.signal_row_selected().connect([&, removeButton](Gtk::ListBoxRow* row) {
        if (row)
        {
            Gtk::Label* label = dynamic_cast<Gtk::Label*>(row->get_child());
            auto it = views.find(label->get_text());
            if (it != views.end())
            {
                viewName = &it->first;
                viewView = &it->second;
                previewWidget.invalidate();
                entryName.set_text(*viewName);
                removeButton->set_sensitive(true);
                //Update preview
                return;
            }
        }
        viewName = nullptr;
        viewView = nullptr;
        entryName.set_text("");
        removeButton->set_sensitive(false);
    });

    for(auto& pair: views)
        addViewRow(list, pair.first);

    if (views.size() > 0)
        list.select_row(*list.get_row_at_index(0));

    Gtk::ScrolledWindow scroll;
    scroll.add(list);
    scroll.set_size_request(300, 200);
    scroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);

    previewWidget.onDraw([&](kj::SFMLWidget& widget) {
        if (viewView)
        {
            auto view = viewView->first;
            view.fit(1.0 * previewWidth / previewHeight);
            Mandelbrot fractal(sf::Vector2u(300, 200));
            fractal.setView(view);
            fractal.iterate(viewView->second);
            fractal.draw(widget.window(), app.getColorScheme());
        }
        widget.display();
    });

    Gtk::HBox hbox;
    hbox.pack_start(scroll);
    hbox.pack_start(previewWidget);

    content->pack_start(hbox);
    content->pack_start(entryBox);

    content->show_all();

    int result = dialog.run();

    connection.disconnect();

    if (result == Gtk::RESPONSE_ACCEPT)
    {
        if (viewView)
        {
            auto view = viewView->first;
            const auto& current = vm.getView();
            view.fit(current.width / current.height);
            vm.setView(view, viewView->second);
        }
    }
}

void showNewDialog(Application& app)
{
    Gtk::Dialog dialog("New Fractal", app.getWindow(), true);

    dialog.add_button("Ok", Gtk::RESPONSE_ACCEPT);

    Gtk::Box* content = dialog.get_content_area();

    Julia* julia = dynamic_cast<Julia*>(&app.getFractal());
    if (julia)
    {
        long double x = julia->getX();
        long double y = julia->getY();

        Gtk::SpinButton xEntry;
        xEntry.set_range(-2.0, 2.0);
        xEntry.set_increments(0.01, 0.1);
        xEntry.set_digits(10);
        xEntry.set_value(x);

        Gtk::SpinButton yEntry;
        yEntry.set_range(-2.0, 2.0);
        yEntry.set_increments(0.01, 0.1);
        yEntry.set_digits(10);
        yEntry.set_value(y);

        content->pack_start(xEntry);
        content->pack_start(yEntry);
        content->show_all();

        int result = dialog.run();
        if (result == Gtk::RESPONSE_ACCEPT)
        {
            x = xEntry.get_value();
            y = yEntry.get_value();
            std::cout << "Woo! " << x << "   " << y << std::endl;
            julia->setValue(x, y);
            app.redrawFractal();
        }
    }
}

void showSetViewDialog(Application& app)
{
    Gtk::Dialog dialog("Set View", app.getWindow(), true);

    dialog.add_button("Ok", Gtk::RESPONSE_ACCEPT);

    Gtk::Box* content = dialog.get_content_area();

    View view = app.getFractal().getView();

    Gtk::SpinButton xEntry;
    xEntry.set_range(-2.0, 2.0);
    xEntry.set_increments(0.01, 0.1);
    xEntry.set_digits(10);
    xEntry.set_value(view.x);

    Gtk::SpinButton yEntry;
    yEntry.set_range(-2.0, 2.0);
    yEntry.set_increments(0.01, 0.1);
    yEntry.set_digits(10);
    yEntry.set_value(view.y);

    content->pack_start(xEntry);
    content->pack_start(yEntry);
    content->show_all();

    int result = dialog.run();
    if (result == Gtk::RESPONSE_ACCEPT)
    {
        view.x = xEntry.get_value();
        view.y = yEntry.get_value();
        app.getFractal().setView(view);
        app.redrawFractal();
    }
}

void addViewRow(Gtk::ListBox& list, const std::string& name)
{
    Gtk::Label* label = Gtk::manage(new Gtk::Label(name));
    list.append(*label);
    list.show_all();
    list.invalidate_sort();
}

void save(const std::string& filename, Fractal& fractal, const ColorScheme& cs, const sf::Vector2u& size, const View& view, int iterations)
{
        std::cout << "Saving " << size << " size image of " << view << " at " << iterations << " iterations to " << filename << std::endl;
        std::cout << "Creating fractal..." << std::endl;
        auto newfractal = fractal.clone(size, view);
        std::cout << "Iterating..." << std::endl;
        int stepCount = 100;
        for(int total = 0; total < iterations;)
        {
            int iterationCount = std::min(stepCount, iterations - total);
            newfractal->iterate(iterationCount);
            total+=iterationCount;
            std::cout << total << std::endl;
        }
        newfractal->setDrawMode(false);

        std::cout << "Creating image..." << std::endl;
        sf::RenderTexture texture;
        if (texture.create(size.x, size.y))
        {
            newfractal->draw(texture, cs);
            texture.display();

            sf::Image image = texture.getTexture().copyToImage();

            std::cout << "Saving image..." << std::endl;
            image.saveToFile(filename);
            std::cout << "Done!" << std::endl;
        }
        else
        {
            std::cout << "Failed to create image!" << std::endl;
        }
}

} // close anonymous namespace
