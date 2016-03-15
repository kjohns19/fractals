#include <fractals/util/View.hpp>
#include <iostream>

View::View()
: View(0, 0, 0, 0) {}
View::View(long double x, long double y, long double width, long double height)
: x(x)
, y(y)
, width(width)
, height(height) {}
View::View(const sf::Vector2<long double>& pos, const sf::Vector2<long double>& size)
: View(pos.x-size.x/2, pos.y-size.y/2, size.x, size.y) {}

void View::fit(const sf::Vector2u& size)
{
    fit(1.0*size.x/size.y);
}
void View::fit(long double ratio)
{
    long double myratio = 1.0 * width / height;

    if (myratio > ratio)
        height = width / ratio;
    else
        width = height * ratio;
}

std::vector<View> View::path(const View& view, int count) const
{
    sf::Vector2<double> startCenter(this->x, this->y);
    sf::Vector2<double> endCenter(view.x, view.y);

    sf::Vector2<double> startSize(this->width, this->height);
    sf::Vector2<double> endSize(view.width, view.height);

    double sizeScale = view.width / this->width;

    sf::Vector2<double> diffCenter = endCenter - startCenter;

    std::vector<View> path;

    double ratio = std::pow(sizeScale, 1.0/(count-1));
    double sum = (std::pow(ratio, count-1) - 1) / (ratio - 1);

    double power = 0.7;

    for(int i = 0; i < count; i++)
    {
        double scale = 1.0 * i / (count - 1);

        double amount = (std::pow(ratio, i) - 1) / (ratio - 1);
        double offScale = std::pow(amount / sum, power);
        std::cout << i << ": " << offScale << std::endl;
        double offX = diffCenter.x * offScale;
        double offY = diffCenter.y * offScale;
        sf::Vector2<double> center = startCenter + sf::Vector2<double>(offX, offY);
        sf::Vector2<double> size = startSize * std::pow(sizeScale, scale);

        path.push_back(View(center.x - size.x/2,
                            center.y - size.y/2,
                            size.x, size.y));
    }

    return path;
}

std::ostream& operator<<(std::ostream& os, const View& view)
{
    return os << "(" << view.x << ", " << view.y
              << ", " << view.width << ", " << view.height << ")";
}
