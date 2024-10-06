#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>

#define MAIN_MENU "main menu"
#define LEVEL_ONE "level one"
#define LEVEL_TWO "level two"
#define END_MENU "end menu"
#define WINDOW_SIZE {1000, 800} // Actual Game Size {600, 400}

using namespace std;

string current_component = MAIN_MENU;
int score = 0;

class Ball {
};

class Cannon {
};

class ChallengeMass {
};

class Button {
};

class Text {
};

class Mouse {
private:
    int position[2] = { 0, 0 };
    sf::Mouse mouse;
    bool left_button_pressed = false;

public:
    Mouse() {
        sf::Mouse mouse();
    }

    void update(sf::RenderWindow& window) {
        sf::Vector2i internal_position = mouse.getPosition(window);
        position[0] = internal_position.x;
        position[1] = internal_position.y;
        left_button_pressed = mouse.isButtonPressed(sf::Mouse::Button::Left);
    }

    int* get_position() {
        return position;
    }

    bool get_left_button_pressed() {
        return left_button_pressed;
    }
};

string main_menu(sf::RenderWindow & window) {
    int score = 0;
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    window.draw(shape);
    return MAIN_MENU;
}

string level_one(sf::RenderWindow & window) {
    return LEVEL_ONE;
}

string level_two(sf::RenderWindow & window) {
    return LEVEL_TWO;
}

string end_menu(sf::RenderWindow & window) {
    return END_MENU;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Bouncing Balls!");
    window.setVerticalSyncEnabled(true);

    Mouse mouse = Mouse();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        window.clear();

        cout << mouse.get_left_button_pressed() << endl;

        if (current_component == MAIN_MENU) {
            current_component = main_menu(window);
        }
        else if (current_component == LEVEL_ONE) {
            current_component = level_one(window);
        }
        else if (current_component == LEVEL_TWO) {
            current_component = level_two(window);
        }
        else if (current_component == END_MENU) {
            current_component = end_menu(window);
        }
        else {
            cout << "Current level not recognised!!!";
            current_component = MAIN_MENU;
        }

        window.display();
        mouse.update(window);
    }

    return 0;
}