#include <fractals/color/ColorScheme.hpp>
#include <fractals/fractal/Fractal.hpp>
#include <fractals/ui/Menu.hpp>
#include <fractals/util/View.hpp>
#include <fractals/ui/ColorSchemeMenu.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/ui/ViewManager.hpp>
#include <fractals/ui/MenuUtils.hpp>
#include <fractals/fractal/Mandelbrot.hpp>
#include <fractals/ui/SFMLWidget.hpp>

#include <SFML/Graphics.hpp>

#include <gtkmm.h>
#include <iostream>
#include <iomanip>

static void showSaveDialog(Application& app);
static void showIterateDialog(Application& app);
static void showViewDialog(Application& app);

static void save(const std::string& filename, const ColorScheme& cs, const sf::Vector2u& size, const View& view, int iterations);

template<typename T>
static std::ostream& operator<<(std::ostream& out, const sf::Vector2<T>& vec)
{
    return out << "(" << vec.x << ", " << vec.y << ")";
}

static std::ostream& operator<<(std::ostream& out, const View& view)
{
    return out << "("
               << view.left << ", "
               << view.top << ", "
               << view.width << ", "
               << view.height << ")";
}


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
        notify(vm);
    }

    ~ViewButtons()
    {
        vm.removeObserver(*this);
    }

    void notify(ViewManager& vm)
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
            save(ss.str(), app.getColorScheme(), app.getWindowSize(), v, iterations);
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
        std::cout << "X: " << view.left + view.width/2 << std::endl;
        std::cout << "Y: " << view.top + view.height/2 << std::endl;
        std::cout << "W: " << view.width << std::endl;
        std::cout << "H: " << view.height << std::endl;
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

        
        save(entryFile->get_text(),
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

static int view_sort_func(Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2) {
    Gtk::Label* label1 = dynamic_cast<Gtk::Label*>(row1->get_child());
    Gtk::Label* label2 = dynamic_cast<Gtk::Label*>(row2->get_child());

    return label1->get_text() < label2->get_text();
}

typedef std::pair<View, int> ViewPair;

static void addViewRow(Gtk::ListBox& list, const std::string& name);

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
    list.set_sort_func(Gtk::ListBox::SlotSort(sigc::ptr_fun(view_sort_func)));

    const int previewWidth = 300;
    const int previewHeight = 200;
    SFMLWidget previewWidget(sf::VideoMode(previewWidth, previewHeight));

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

    previewWidget.onDraw([&](SFMLWidget& widget) {
        if (viewView)
        {
            auto view = viewView->first;
            view.fit(1.0 * previewWidth / previewHeight);
            Mandelbrot fractal(sf::Vector2u(300, 200), view);
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

static void addViewRow(Gtk::ListBox& list, const std::string& name)
{
    Gtk::Label* label = Gtk::manage(new Gtk::Label(name));
    list.append(*label);
    list.show_all();
    list.invalidate_sort();
}

static void save(const std::string& filename, const ColorScheme& cs, const sf::Vector2u& size, const View& view, int iterations)
{
        std::cout << "Saving " << size << " size image of " << view << " at " << iterations << " iterations to " << filename << std::endl;
        std::cout << "Creating fractal..." << std::endl;
        Mandelbrot fractal(size, view);
        std::cout << "Iterating..." << std::endl;
        int stepCount = 100;
        for(int total = 0; total < iterations;)
        {
            int iterationCount = std::min(stepCount, iterations - total);
            fractal.iterate(iterationCount);
            total+=iterationCount;
            std::cout << total << std::endl;
        }
        fractal.setDrawMode(false);

        std::cout << "Creating image..." << std::endl;
        sf::RenderTexture texture;
        if (texture.create(size.x, size.y))
        {
            fractal.draw(texture, cs);
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
