#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>

#define MAIN_MENU "main menu"
#define LEVEL_ONE "level one"
#define LEVEL_TWO "level two"
#define END_MENU "end menu"
#define WINDOW_SIZE {1000, 800} // Actual Game Size {600, 400}

#define ENTER_KEY 58
#define ESCAPE_KEY 36

using namespace std;

sf::Color pick_ball_color() {
    int randomly_chosen_color = rand() % 3;
    if (randomly_chosen_color == 0) {
        return sf::Color::Blue;
    }
    else if (randomly_chosen_color == 1) {
        return sf::Color::Yellow;
    }
    else {
        return sf::Color::Red;
    }
}

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
        sf::Vector2i internal_position = mouse.getPosition(
            window);
        position[0] = internal_position.x;
        position[1] = internal_position.y;
        left_button_pressed = mouse.isButtonPressed(
            sf::Mouse::Button::Left);
    }

    int* get_position() {
        return position;
    }

    bool get_left_button_pressed() {
        return left_button_pressed;
    }

    void set_left_button_pressed(bool value) {
        left_button_pressed = value;
    }
};

class MainMenu {
public:
    MainMenu() {
    }

    string run_menu(sf::RenderWindow& window) {
        int score = 0;
        sf::CircleShape shape(100.f);
        shape.setFillColor(pick_ball_color());
        window.draw(shape);
        return MAIN_MENU;
    }

};

class LevelOne {
public:
    LevelOne() {
    }

    string run_menu(sf::RenderWindow& window) {
        return LEVEL_ONE;
    }

};

class LevelTwo {
public:
    LevelTwo() {
    }

    string run_menu(sf::RenderWindow& window) {
        return LEVEL_TWO;
    }
};

class EndMenu {
public:
    EndMenu() {
    }

    string run_menu(sf::RenderWindow& window) {
        return END_MENU;
    }
};

int main()
{
    // setup local variables
    string menu_navigation[] = { MAIN_MENU, MAIN_MENU };
    /*
    * The Menu Navigation System, created in the variable menu_navigation, allows us
    * to easily move between the current menu and the previous menu - so that the
    * player can return to the main menu at any time or a pause menu and the game can
    * "remember" what was happening befvore this switch occured. In a larger game
    * the array could be larger as there are more menus to navigate through,
    * but I am keeping this relatively simple and small.
    */
    int score = 0;
    string next_component;

    // setup game componentry
    Mouse mouse = Mouse();

    // setup game levels
    MainMenu main_menu = MainMenu();
    LevelOne level_one = LevelOne();
    LevelTwo level_two = LevelTwo();
    EndMenu end_menu = EndMenu();

    // create window (do last so not unresponsive whilst the game loads)
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Bouncing Balls!");
    window.setVerticalSyncEnabled(true); // vsync is our frame limiter here.
    // No need for clocks!

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            } else if(event.type == sf::Event::KeyPressed) {
                if (event.key.code == ENTER_KEY) {
                    mouse.set_left_button_pressed(true);
                }
                else if (event.key.code == ESCAPE_KEY) {
                    string current_menu = menu_navigation[0];
                    string menu_navigation[] = { MAIN_MENU, current_menu };
                }
            }
        }

        window.clear();

        if (menu_navigation[0] == MAIN_MENU) {
            next_component = main_menu.run_menu(window);
            if (next_component != MAIN_MENU) {
                string menu_navigation[] = { next_component, MAIN_MENU };
            }
        }
        else if (menu_navigation[0] == LEVEL_ONE) {
            next_component = level_one.run_menu(window);
            if (next_component != LEVEL_ONE) {
                string menu_navigation[] = { next_component, LEVEL_ONE };
            }
        }
        else if (menu_navigation[0] == LEVEL_TWO) {
            next_component = level_two.run_menu(window);
            if (next_component != LEVEL_ONE) {
                string menu_navigation[] = { next_component, LEVEL_TWO };
            }
        }
        else if (menu_navigation[0] == END_MENU) {
            next_component = end_menu.run_menu(window);
            if (next_component != END_MENU) {
                string menu_navigation[] = { next_component, END_MENU };
            }
        }
        else {
            cout << "Current level not recognised!!!";
            menu_navigation[0] = MAIN_MENU;
            menu_navigation[1] = MAIN_MENU;
        }

        window.display();
        mouse.update(window);
    }

    return 0;
}