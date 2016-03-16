#include <fractals/ui/ColorSchemeMenu.hpp>
#include <fractals/ui/SFMLWidget.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/ui/MenuUtils.hpp>
#include <fractals/color/ColorScheme.hpp>
#include <kj/gtkmm_no_warning.hpp>
#include <SFML/Graphics.hpp>
#include <utility>
#include <iostream>

typedef std::tuple<Gtk::SpinButton*, Gtk::ColorButton*, Gtk::Button*> Item;

namespace {

void getColors(const std::vector<Item>& items, std::map<double, sf::Color>& colors);

sf::Color getColor(const Gtk::ColorButton* button);

void addRow(Gtk::ListBox& list,
            double value, const sf::Color& color,
            SFMLWidget& previewWidget,
            std::vector<Item>& items,
            std::map<Gtk::Button*, std::pair<Gtk::ListBoxRow*, int> >& itemMap);

int listSortFunc(Gtk::ListBoxRow* row1, Gtk::ListBoxRow* row2)
{
    Gtk::HBox* box1 = dynamic_cast<Gtk::HBox*>(row1->get_child());
    Gtk::HBox* box2 = dynamic_cast<Gtk::HBox*>(row2->get_child());

    auto entry1 = dynamic_cast<Gtk::SpinButton*>(box1->get_children()[0]);
    auto entry2 = dynamic_cast<Gtk::SpinButton*>(box2->get_children()[0]);
    return entry1->get_value() < entry2->get_value();
}

} // close anonymous namespace

void showColorSchemeMenu(Application& app)
{
    Gtk::Dialog dialog("Color Scheme", app.getWindow(), true);

    ColorScheme& cs = dynamic_cast<ColorScheme&>(app.getColorScheme());

    dialog.add_button("Ok", Gtk::RESPONSE_OK);
    dialog.add_button("Apply", Gtk::RESPONSE_APPLY);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

    const std::map<double, sf::Color>& colors = cs.getColors();

    int startLoopCount = cs.getLoopCount();

    Gtk::Box* content = dialog.get_content_area();

    Gtk::ListBox list;

    list.set_sort_func(sigc::ptr_fun(listSortFunc));
    list.signal_row_selected().connect([&](Gtk::ListBoxRow* row) {
        list.unselect_row();
    });

    std::vector<Item> items;
    std::map<Gtk::Button*, std::pair<Gtk::ListBoxRow*, int> > itemMap;

    SFMLWidget previewWidget(sf::VideoMode(300, 100));

    for(auto& colorPair: colors)
        addRow(list, colorPair.first, colorPair.second, previewWidget, items, itemMap);

    if (colors.size() == 1)
        std::get<2>(items[0])->set_sensitive(false);

    Gtk::ScrolledWindow scroll;
    scroll.add(list);
    scroll.set_size_request(300, 200);
    scroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
    content->pack_start(scroll);

    Gtk::SpinButton* loopEntry = Gtk::manage(new Gtk::SpinButton());

    previewWidget.onDraw([&](SFMLWidget& widget) {
        sf::RenderTarget& target = widget.window();

        std::map<double, sf::Color> newColors;
        getColors(items, newColors);
        ColorScheme newCS(newColors);
        newCS.setLoopCount(loopEntry->get_value_as_int());

        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(1, 100));
        for(int i = 0; i < 300; i++)
        {
            rect.setPosition(i, 0);
            rect.setFillColor(newCS.getColor(1.0*i/300));
            target.draw(rect);
        }
        widget.display();
    });

    content->pack_start(previewWidget);

    createButton(content, "Spread", "gtk-sort-descending", [&]() {
        std::vector<Gtk::SpinButton*> values;
        for(auto& item: items)
            values.push_back(std::get<0>(item));
        std::sort(values.begin(), values.end(),
            [](const Gtk::SpinButton* b1, const Gtk::SpinButton* b2) {
                return b1->get_value() < b2->get_value();
            });

        int i = 0;
        for(auto value: values)
            value->set_value(1.0 * (i++) / (values.size()-1));
    });

    createButton(content, "Reset", "gtk-clear", [&]() {
        std::vector<Gtk::Widget*> children = list.get_children();
        for(auto child: children)
            list.remove(*child);
        for(auto& pair: colors)
            addRow(list, pair.first, pair.second, previewWidget, items, itemMap);
        loopEntry->set_value(startLoopCount);
        list.invalidate_sort();
        list.show_all();
        previewWidget.invalidate();
    });

    loopEntry->set_range(1, 1000);
    loopEntry->set_increments(1, 10);
    loopEntry->set_value(cs.getLoopCount());
    loopEntry->signal_value_changed().connect([&]() {
        previewWidget.invalidate();
    });
    content->pack_start(*loopEntry);

    content->show_all();
    
    while(1)
    {
        int result = dialog.run();

        if (result == Gtk::RESPONSE_OK || result == Gtk::RESPONSE_APPLY)
        {
            std::map<double, sf::Color> newColors;
            getColors(items, newColors);
            cs.set(newColors);
            cs.setLoopCount(loopEntry->get_value_as_int());
            app.redrawFractal();

            if (result == Gtk::RESPONSE_OK)
                break;
        }
        else
            break;
    }
}

namespace {

void addRow(Gtk::ListBox& list,
            double value, const sf::Color& color,
            SFMLWidget& previewWidget,
            std::vector<Item>& items,
            std::map<Gtk::Button*, std::pair<Gtk::ListBoxRow*, int> >& itemMap)
{
    Gtk::HBox* box = Gtk::manage(new Gtk::HBox());
    Gtk::SpinButton* valueEntry = Gtk::manage(new Gtk::SpinButton());
    valueEntry->set_range(0, 1);
    valueEntry->set_digits(3);
    valueEntry->set_increments(0.01, 0.1);
    valueEntry->set_value(value);

    box->pack_start(*valueEntry);

    Gtk::ColorButton* colorButton = Gtk::manage(new Gtk::ColorButton());

    Gdk::Color gdkColor;
    gdkColor.set_rgb(color.r*255, color.g*255, color.b*255);
    colorButton->set_color(gdkColor);
    colorButton->signal_color_set().connect([&]() {
        previewWidget.invalidate();
    });
    box->pack_start(*colorButton);

    Gtk::Button* removeButton = createButton("Remove", "gtk-remove");
    removeButton->signal_clicked().connect(
        [removeButton, &list, &itemMap, &items]() {
            auto it = itemMap.find(removeButton);
            if (it != itemMap.end()) //Shouldn't happen
            {
                Gtk::ListBoxRow* row = it->second.first;
                int pos = it->second.second;

                items[pos] = items.back();
                itemMap[std::get<2>(items[pos])].second = pos;
                items.pop_back();

                itemMap.erase(it);
                if (items.size() == 1)
                    std::get<2>(items[0])->set_sensitive(false);

                list.remove(*row);
                delete row;
            }
        });
    box->pack_start(*removeButton);

    createButton(box, "Copy", "gtk-copy", [valueEntry, colorButton, &list, &previewWidget, &items, &itemMap]() {
        double value = valueEntry->get_value();
        sf::Color color = getColor(colorButton);
        addRow(list, value, color, previewWidget, items, itemMap);
        list.invalidate_sort();
        list.show_all();
        previewWidget.invalidate();
        if (items.size() == 2)
        {
            for(auto& item: items)
                std::get<2>(item)->set_sensitive(true);
        }
    });

    valueEntry->signal_value_changed().connect([&]() {
        list.invalidate_sort();
        previewWidget.invalidate();
    });

    Gtk::ListBoxRow* listRow = Gtk::manage(new Gtk::ListBoxRow());
    listRow->add(*box);

    list.add(*listRow);
    itemMap[removeButton] = std::make_pair(listRow, (int)items.size());
    items.push_back(Item(valueEntry, colorButton, removeButton));
}

void getColors(const std::vector<Item>& items, std::map<double, sf::Color>& colors)
{
    for(auto& item: items)
    {
        double value = std::get<0>(item)->get_value();
        colors[value] = getColor(std::get<1>(item));
    }
}

sf::Color getColor(const Gtk::ColorButton* button)
{
    Gdk::Color color = button->get_color();
    return sf::Color((int)color.get_red()/256,
                     (int)color.get_green()/256,
                     (int)color.get_blue()/256);
}

} // close anonymous namespace
