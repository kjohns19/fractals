#include <fractals/ui/color_scheme_editor.hpp>

#include <fractals/color/color_scheme.hpp>
#include <fractals/util/builder_utils.hpp>
#include <fractals/util/raii_helpers.hpp>
#include <fractals/util/sfml_widget.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <gdkmm/rgba.h>
#include <gtkmm/box.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/dialog.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/listbox.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/toggletoolbutton.h>

#include <map>

namespace frac {
namespace {


void addRow(
        Gtk::ListBox& box, SFMLWidget& preview,
        double position, const Gdk::RGBA& color)
{
    using BU = BuilderUtils;
    auto builder = Gtk::Builder::create();
    builder->add_from_file("data/colorscheme-row.glade");

    auto& positionEntry = BU::getWidget<Gtk::SpinButton>(
        builder, "entry-position");
    positionEntry.set_value(position);
    positionEntry.signal_value_changed().connect([&box, &preview]() {
        preview.invalidate();
        box.invalidate_sort();
    });

    auto& colorEntry = BU::getWidget<Gtk::ColorButton>(
        builder, "entry-color");
    colorEntry.set_rgba(color);
    colorEntry.signal_color_set().connect([&preview]() {
        preview.invalidate();
    });

    BU::getWidget<Gtk::Button>(builder, "button-add")
    .signal_clicked().connect([&box, &preview, &positionEntry, &colorEntry]() {
        double position = positionEntry.get_value();
        Gdk::RGBA color = colorEntry.get_rgba();
        addRow(box, preview, position, color);
        preview.invalidate();
    });

    auto& row = BU::getWidget<Gtk::Box>(builder, "row");

    BU::getWidget<Gtk::Button>(builder, "button-remove")
    .signal_clicked().connect([&box, &preview, &row]() {
        box.remove(row);
        preview.invalidate();
    });

    box.add(row);
}

void addRow(
        Gtk::ListBox& box, SFMLWidget& preview,
        double position, const sf::Color& color)
{
    Gdk::RGBA rgba;
    auto convert = [](unsigned char value) -> unsigned short {
        return (256*256-1)*value/255;
    };
    rgba.set_rgba_u(convert(color.r), convert(color.g), convert(color.b));
    addRow(box, preview, position, rgba);
}

void fillColorList(
        Gtk::ListBox& box, Gtk::SpinButton& repeatSpinner,
        SFMLWidget& preview, const ColorScheme& colorScheme)
{
    for(auto widget: box.get_children())
        box.remove(*widget);

    for(auto& pair: colorScheme)
        addRow(box, preview, pair.first, pair.second);

    repeatSpinner.set_value(colorScheme.getLoopCount());
}

ColorScheme buildColorScheme(
        Gtk::ListBox& box, Gtk::SpinButton& repeatSpinner)
{
    ColorScheme colorScheme;

    box.foreach([&colorScheme](Gtk::Widget& widget) {
        auto& row = dynamic_cast<Gtk::ListBoxRow&>(widget);
        auto& box = dynamic_cast<Gtk::Box&>(*row.get_child());
        auto children = box.get_children();
        double position = dynamic_cast<Gtk::SpinButton&>(*children[0])
            .get_value();
        Gdk::RGBA rgba = dynamic_cast<Gtk::ColorButton&>(*children[1])
            .get_rgba();
        auto convert = [](unsigned short value) -> unsigned char {
            return value*255/(256*256-1);
        };
        sf::Color color(
            convert(rgba.get_red_u()),
            convert(rgba.get_green_u()),
            convert(rgba.get_blue_u()));
        colorScheme.add(position, color);
    });
    colorScheme.setLoopCount(repeatSpinner.get_value_as_int());
    return colorScheme;
}

}

void ColorSchemeEditor::configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& idPrefix,
        const std::string& toolId)
{
    using BU = BuilderUtils;

    auto& colorDialog = BU::getWidget<Gtk::Dialog>(
        builder, idPrefix);
    auto& repeatSpinner = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-repeat-spinner");

    auto& listBox = BU::getWidget<Gtk::ListBox>(builder, idPrefix + "-list");
    listBox.set_sort_func([](Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2) {
        auto& box1 = dynamic_cast<Gtk::Box&>(*row1->get_child());
        auto& box2 = dynamic_cast<Gtk::Box&>(*row2->get_child());

        auto& entry1 = dynamic_cast<Gtk::SpinButton&>(*box1.get_children()[0]);
        auto& entry2 = dynamic_cast<Gtk::SpinButton&>(*box2.get_children()[0]);
        int diff = entry1.get_value() < entry2.get_value();
        return (diff < 0) ? -1 : (diff > 0 ? 1 : 0);
    });

    sf::Vector2u previewSize(300u, 100u);
    auto previewWidget = Gtk::manage(
        new SFMLWidget({previewSize.x, previewSize.y}));
    previewWidget->onDraw(
    [&listBox, &repeatSpinner, previewSize](SFMLWidget& widget) {
        sf::RenderTarget& target = widget.window();

        ColorScheme colorScheme = buildColorScheme(listBox, repeatSpinner);

        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(1, previewSize.y));
        for(unsigned i = 0; i < previewSize.x; i++)
        {
            rect.setPosition(i, 0);
            rect.setFillColor(colorScheme.getColor(1.0*i/previewSize.x));
            target.draw(rect);
        }
    });
    previewWidget->show();
    BU::getWidget<Gtk::Box>(builder, idPrefix + "-preview-area")
    .pack_start(*previewWidget, Gtk::PACK_SHRINK);

    repeatSpinner.signal_value_changed().connect([previewWidget]() {
        previewWidget->invalidate();
    });


    BU::getWidget<Gtk::ToolButton>(builder, toolId)
    .signal_clicked().connect(
    [&app, previewWidget, &colorDialog, &listBox, &repeatSpinner]() {
        PauseFractal pause(app.fractalWidget());

        fillColorList(
            listBox, repeatSpinner, *previewWidget, app.colorScheme());

        int result;
        do
        {
            result = colorDialog.run();
            if (result == Gtk::RESPONSE_APPLY || result == Gtk::RESPONSE_OK)
            {
                app.colorScheme() = buildColorScheme(listBox, repeatSpinner);
                app.fractalWidget().redraw();
            }
        }
        while (result == Gtk::RESPONSE_APPLY);

        colorDialog.hide();
    });
}

} // close namespace frac
