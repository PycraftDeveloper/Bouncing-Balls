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

#define FONT_DIRECTORY "G:\\University Assignments\\CSY1079 - Programming 2D Computer Games\\AS1-Bouncing-Balls\\AS1 - Bouncing Balls - Visual Studio Project\\fonts"
#define NOTO_SANS_JP_FONT_PATH FONT_DIRECTORY + "\\Noto_Sans JP\\NotoSansJP - Regular.ttf"

using namespace std;

// define global CONSTANTS
string PROJECT_RESOURCES_DIRECTORY = "G:\\University\\Assignments\\Simester 1\\CSY1079 - Programming 2D Computer Games\\AS1 - Bouncing Balls - Visual Studio Project";
const float TAU = 3.14159265358979323 * 2;

// define global VARIABLES
int window_size[] = { 1280, 720 };

float pythagorean_distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int calculate_point_count(int radius, float shape_quality = 0.75) {
    if (radius < 1) {
        return 3;
    }
    float angle = asin(1.0f / radius);
    int point_count = 1 + int((TAU / angle) * shape_quality);

    if (point_count < 3) {
        point_count = 3;
    }

    return point_count;
}

string path_builder(string (&args)[50]) {
    string resultant_path = PROJECT_RESOURCES_DIRECTORY;
    int length_of_input_array = sizeof(args) / sizeof(args[0]);
    for (int i = 0; i < length_of_input_array; i++) {
        if (args[i] == "") {
            break;
        }
        resultant_path += "\\" + args[i];
    }
    return resultant_path;
}

void play_random_pop_sounds(sf::Music* pop_sounds, int count) {
    for (int i = 0; i < count; i++) {
        int sound_index = rand() % 15;
        pop_sounds[sound_index].play();
    }
    // it is possible to do this serially. By creating custom music objects with a play function that mimics the music.play function and then updating these objects every frame with custom delays.
    // Note: doing this would require that the audio isn't already in its delay state which could easily be added in. This limits max number of sounds to 15, as that's how many unique files we have,
    // however clever sound design should be able to mask this limitation.
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
public:
    float x, y;
    float mass;
    sf::Color color;
    float x_speed, y_speed;
    sf::CircleShape shape;
    bool has_drag;
    bool can_bounce;

    Ball(float x, float y, float mass, sf::Color color, bool has_drag, bool can_bounce) : x(x), y(y), mass(mass), color(color), has_drag(has_drag), can_bounce(can_bounce) {
        x_speed = (rand() % 15) + 1;
        y_speed = (rand() % 15) + 1;

        shape.setFillColor(color);
        shape.setRadius(mass);
        shape.setOrigin(mass, mass); // Centre the origin for proper positioning
        shape.setPointCount(calculate_point_count(mass));
    }

    void compute() {
        x += x_speed;
        y += y_speed;

        // Correct the boundary collision to respect ball's radius
        if ((x - mass <= 0 && x_speed < 0) || (x + mass >= window_size[0] && x_speed > 0)) {
            x_speed = -x_speed;
        }
        if ((y - mass <= 0 && y_speed < 0) || (y + mass >= window_size[1] && y_speed > 0)) {
            y_speed = -y_speed;
        }

        // Keep the ball inside the window
        if (x - mass < 0) x = mass;
        if (x + mass > window_size[0]) x = window_size[0] - mass;
        if (y - mass < 0) y = mass;
        if (y + mass > window_size[1]) y = window_size[1] - mass;

        if (has_drag) {
            x_speed *= 0.99;
            y_speed *= 0.99;
        }
    }

    void collision(Ball& ball) {
        // Calculate the distance between the two balls
        float distance = pythagorean_distance(x, y, ball.x, ball.y);
        if (distance < mass + ball.mass) {
            // Calculate the angle of the collision
            float angle = atan2(y - ball.y, x - ball.x);
            float sin_angle = sin(angle);
            float cos_angle = cos(angle);

            // Move the balls apart to prevent overlap
            float overlap = 0.5f * (mass + ball.mass - distance + 1); // Slight overlap prevention
            x += cos_angle * overlap;
            y += sin_angle * overlap;
            ball.x -= cos_angle * overlap;
            ball.y -= sin_angle * overlap;

            if (can_bounce) {
                // Rotate the velocities so that the collision is head-on
                float v1_x_rot = cos_angle * x_speed + sin_angle * y_speed;
                float v1_y_rot = cos_angle * y_speed - sin_angle * x_speed;
                float v2_x_rot = cos_angle * ball.x_speed + sin_angle * ball.y_speed;
                float v2_y_rot = cos_angle * ball.y_speed - sin_angle * ball.x_speed;

                // Apply the 1D collision equations along the rotated axis
                float v1_x_rot_new = (v1_x_rot * (mass - ball.mass) + 2 * ball.mass * v2_x_rot) / (mass + ball.mass);
                float v2_x_rot_new = (v2_x_rot * (ball.mass - mass) + 2 * mass * v1_x_rot) / (mass + ball.mass);

                // Apply component velocities to ball
                x_speed = cos_angle * v1_x_rot_new - sin_angle * v1_y_rot;
                y_speed = sin_angle * v1_x_rot_new + cos_angle * v1_y_rot;
                ball.x_speed = cos_angle * v2_x_rot_new - sin_angle * v2_y_rot;
                ball.y_speed = sin_angle * v2_x_rot_new + cos_angle * v2_y_rot;
            }
            else {
                // Apply component velocities to ball
                x_speed = 0;
                y_speed = 0;
                ball.x_speed = 0;
                ball.y_speed = 0;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        shape.setPosition(x, y);
        window.draw(shape);
    }
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

class GameEndMenu {
public:
    GameEndMenu() {
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
    GameEndMenu end_menu = GameEndMenu();
    PauseMenu pause_menu = PauseMenu();

    // create window (do last so not unresponsive whilst the game loads)
    sf::RenderWindow window(sf::VideoMode(window_size[0], window_size[1]), "Bouncing Balls!");
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

    play_random_pop_sounds(pop_sounds, rand() % 15);

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