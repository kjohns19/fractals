#include <fractals/color/ColorScheme.hpp>
#include <fractals/fractal/Fractal.hpp>
#include <fractals/ui/Menu.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/ui/ViewManager.hpp>
#include <fractals/fractal/Mandelbrot.hpp>

#include <SFML/Graphics.hpp>

#include <gtkmm.h>
#include <iostream>

template<typename ButtonType = Gtk::Button>
ButtonType* createButton(const std::string& label, const std::string& stock);
template<typename ButtonType = Gtk::Button>
ButtonType* createButton(Gtk::Box* menu, const std::string& label, const std::string& stock, const std::function<void()>& func);

void showSaveDialog(Application& app);

struct ViewButtons : public Gtk::HBox, public Observer<ViewManager>
{
    ViewButtons(Application& app):
        vm(app.getViewManager())
    {
        firstButton = createButton(this, "First View",
            "gtk-goto-first-ltr", [this]() {
                vm.firstView();
            });
        prevButton = createButton(this, "Previous View",
            "gtk-go-back-ltr", [this]() {
                vm.previousView();
            });
        nextButton = createButton(this, "Next View",
            "gtk-go-forward-ltr", [this]() {
                vm.nextView();
            });
        lastButton = createButton(this, "Last View",
            "gtk-goto-last-ltr", [this]() {
                vm.lastView();
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
        firstButton->set_sensitive(vm.hasPreviousView());
        prevButton->set_sensitive(vm.hasPreviousView());
        nextButton->set_sensitive(vm.hasNextView());
        lastButton->set_sensitive(vm.hasNextView());
    }
    ViewManager& vm;
    Gtk::Button* firstButton;
    Gtk::Button* prevButton;
    Gtk::Button* nextButton;
    Gtk::Button* lastButton;
};


template<typename ButtonType>
ButtonType* createButton(const std::string& label, const std::string& stock)
{
    ButtonType* button;

    button = Gtk::manage(new ButtonType());

    button->set_image_from_icon_name(stock, Gtk::ICON_SIZE_BUTTON);
    button->set_tooltip_text(label);

    return button;
}

template<typename ButtonType>
ButtonType* createButton(Gtk::Box* menu, const std::string& label, const std::string& stock, const std::function<void()>& func)
{
    ButtonType* button = createButton<ButtonType>(label, stock);
    button->signal_clicked().connect(func);
    menu->pack_start(*button);
    return button;
}

Gtk::Widget* createMenu(Application& app)
{
    Gtk::Box* menu = Gtk::manage(new Gtk::ButtonBox());

    createButton(menu, "Save", "gtk-save", [&]() {
        showSaveDialog(app);
    });

    createButton(menu, "Reset", "gtk-refresh", [&]() {
        app.getViewManager().resetView();
    });

    ViewButtons* vbuttons = Gtk::manage(new ViewButtons(app));
    menu->pack_start(*vbuttons);

    Gtk::ToggleButton* button = createButton<Gtk::ToggleButton>("Fast Drawing", "gtk-edit");
    button->set_active(true);
    button->signal_clicked().connect([&]() {
        app.getFractal().setDrawMode(!app.getFractal().getDrawMode());
        app.redrawFractal();
    });
    menu->pack_start(*button);

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

    row = Gtk::manage(new Gtk::HBox());
    row->pack_start(*Gtk::manage(new Gtk::Label("Iterations: ", Gtk::ALIGN_END)));
    entryIterations = Gtk::manage(new Gtk::SpinButton());
    entryIterations->set_range(1, 99999);
    entryIterations->set_increments(1, 10);
    entryIterations->set_value(app.getFractal().iterations());
    row->pack_start(*entryIterations);
    content->pack_start(*row);


    /*
    //Height
    row = Gtk::manage(new Gtk::HBox());
    label = Gtk::manage(new Gtk::Label("Height:", Gtk::ALIGN_END));
    row->pack_start(*label);
    row->pack_start(*entryHeight);
    content->pack_start(*row);
    */


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
