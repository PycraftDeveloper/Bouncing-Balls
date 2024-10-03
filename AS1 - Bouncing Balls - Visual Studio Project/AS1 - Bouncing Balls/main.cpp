#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#define MAIN_MENU "main menu"
#define LEVEL_ONE "level one"
#define LEVEL_TWO "level two"
#define END_MENU "end menu"

using namespace std;

int main()
{   
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    window.setVerticalSyncEnabled(true);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}