#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>

#define MAIN_MENU "main menu"
#define LEVEL_ONE "level one"
#define LEVEL_TWO "level two"
#define END_MENU "end menu"
#define PAUSE_MENU "pause menu"
#define WINDOW_SIZE {1000, 800} // Actual Game Size {600, 400}

#define FONT_DIRECTORY "G:\\University Assignments\\CSY1079 - Programming 2D Computer Games\\AS1-Bouncing-Balls\\AS1 - Bouncing Balls - Visual Studio Project\\fonts"
#define NOTO_SANS_JP_FONT_PATH FONT_DIRECTORY + "\\Noto_Sans JP\\NotoSansJP - Regular.ttf"

using namespace std;

string PROJECT_RESOURCES_DIRECTORY = "G:\\University Assignments\\CSY1079 - Programming 2D Computer Games\\AS1-Bouncing-Balls\\AS1 - Bouncing Balls - Visual Studio Project";

string path_builder(string (&args)[50]) {
    string resultant_path = PROJECT_RESOURCES_DIRECTORY;
    int length_of_input_array = sizeof(args) / sizeof(args[0]);
    for (int i = 0; i < length_of_input_array; i++) {
        if (args[i] == "") break;
        resultant_path += "\\" + args[i];
    }
    return resultant_path;
}

void play_random_pop_sounds(sf::Music* pop_sounds, int count) {
    for (int i = 0; i < count; i++) {
        int sound_index = rand() % 15;
        pop_sounds[sound_index].play();
    }
}

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
    int rotation;
    string game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
public:
    MainMenu() {
        rotation = (rand() % 90)-45;
    }

    string run_menu(sf::RenderWindow& window) {
        sf::Text text;
        sf::Font font;
        string path_components[50] = { "Fonts", "Noto_Sans JP", "NotoSansJP-Regular.ttf" };
        font.loadFromFile(path_builder(path_components));

        // select the font
        text.setFont(font); // font is a sf::Font

        // set the string to display
        text.setString(L"パズルボブル");

        // set the character size
        text.setCharacterSize(300); // in pixels, not points!
           
        // set text rotation
        text.rotate(rotation);

        // get text size
        int text_x_size = text.getLocalBounds().width;
        int text_y_size = text.getLocalBounds().height;

        // set text position
        text.setPosition(500, 500);

        // set the color
        text.setFillColor(sf::Color(225, 225, 225));

        int score = 0;
        sf::CircleShape shape(100.f);
        shape.setFillColor(pick_ball_color());
        shape.setPosition({ 100, 200 });
        window.draw(shape);
        window.draw(text);
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

    void reset_level() {
    }

};

class LevelTwo {
public:
    LevelTwo() {
    }

    string run_menu(sf::RenderWindow& window) {
        return LEVEL_TWO;
    }

    void reset_level() {
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

class PauseMenu {
public:
    PauseMenu() {
    }

    string run_menu(sf::RenderWindow& window, string *menu_navigation) {
        return menu_navigation[1]; // go to previous level
        return PAUSE_MENU; // stay on current window alternate between these for menu navigation
    }
};

int main()
{
    srand(time(0)); // enforces random values using seed of current time.

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
    PauseMenu pause_menu = PauseMenu();

    // create window (do last so not unresponsive whilst the game loads)
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Bouncing Balls!");
    window.setVerticalSyncEnabled(true); // vsync is our frame limiter here.
    // No need for clocks!

    // Load pop sounds. Using music here means files arent actually loaded
    // and instead streamed as needed, meaning this isnt actually that
    // inefficient!
    sf::Music pop_sounds[15];
    for (int i = 0; i < 15; i++) { // 16
        string file = to_string(i+1) + ".wav";
        string path_components[50] = { "resources", "sounds", "pops", file };
        string file_path = path_builder(path_components);
        pop_sounds[i].openFromFile(file_path);
    }

    play_random_pop_sounds(pop_sounds, 13);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            } else if(event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    mouse.set_left_button_pressed(true);
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    string current_menu = menu_navigation[0];
                    string menu_navigation[] = { MAIN_MENU, current_menu };
                }
            }
        }

        window.clear(sf::Color(255, 255, 255));

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
                if (next_component != PAUSE_MENU) {
                    level_two.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == LEVEL_TWO) {
            next_component = level_two.run_menu(window);
            if (next_component != LEVEL_ONE) {
                string menu_navigation[] = { next_component, LEVEL_TWO };
                if (next_component != PAUSE_MENU) {
                    level_two.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == END_MENU) {
            next_component = end_menu.run_menu(window);
            if (next_component != END_MENU) {
                string menu_navigation[] = { next_component, END_MENU };
            }
        }
        else if (menu_navigation[0] == PAUSE_MENU) {
            next_component = pause_menu.run_menu(window, menu_navigation);
            if (next_component != PAUSE_MENU) {
                string menu_navigation[] = { next_component, PAUSE_MENU };
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