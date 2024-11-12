#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>
#include <cmath>

#define MAIN_MENU "main menu"
#define LEVEL_ONE "level one"
#define LEVEL_TWO "level two"
#define END_MENU "end menu"
#define PAUSE_MENU "pause menu"

#define QUIT "quit"

#define FONT_PLAY "play font"
#define FONT_REGULAR "play regular"

#define WIN "won"
#define LOST "lost"

#define BALL_SPEED 3

#define BALL_RADIUS 20

using namespace std;

// define global CONSTANTS
string PROJECT_RESOURCES_DIRECTORY = "G:\\University\\Assignments\\Simester 1\\CSY1079 - Programming 2D Computer Games\\AS1 - Bouncing Balls - Visual Studio Project";
const float PI = 3.14159265358979323;
const float TAU = PI * 2;
const float RADIANS_TO_DEGREES_CONVERSION_CONSTANT = 180 / PI;
const float DEGREES_TO_RADIANS_CONVERSION_CONSTANT = PI / 180;

// define global VARIABLES
int window_size[] = { 1280, 720 };
string game_end_state = "won";

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

string path_builder(string(&args)[50]) { // currently length 50, lower
    // this to match longest path for memory savings.

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
    // it is possible to do this serially. By creating custom music objects 
    // with a play function that mimics the music.play function and then 
    // updating these objects every frame with custom delays.
    // Note: doing this would require that the audio isn't already in its 
    // delay state which could easily be added in. This limits max number 
    // of sounds to 15, as that's how many unique files we have,
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

class PlayerInput {
private:
    int mouse_position[2] = { 0, 0 };
    sf::Mouse mouse;
    bool player_button_input = false;
    bool player_continual_interation = false;


public:
    PlayerInput() {
        sf::Mouse mouse();
    }

    void update(sf::RenderWindow& window) {
        sf::Vector2i internal_position = mouse.getPosition(
            window);
        mouse_position[0] = internal_position.x;
        mouse_position[1] = internal_position.y;
        player_button_input = mouse.isButtonPressed(
            sf::Mouse::Button::Left);

        // enforce that when the player is not releasing the input quickly enough
        // the player input will be reset to false until the player releases the
        // input. This is to prevent the illusion of clicking through an object
        // on one menu through another when two clickable objects are in the same place
        // on different levels but clicking one causes the other to also be pressed.
        // for example: clicking the "play again" button in the end screen resets the menu
        // system back to the main menu. But if the player isn't fast enough this same input
        // causes the "play" button, which is in the same place on-screen, to also then
        // get pressed meaning to the end user, pressing the play again button dumps
        // you right to the start of level 1, instead of the main menu.

        if (player_button_input && player_continual_interation) {
            player_button_input = false;
        }
        else if (player_button_input) {
            player_continual_interation = true;
        }
        else {
            player_continual_interation = false;
        }
    }

    int* get_mouse_position() {
        return mouse_position;
    }

    bool get_player_button_input() {
        return player_button_input;
    }

    void set_player_button_input(bool value) {
        player_button_input = value;
    }
};

class Mass {
public:
    sf::Texture mass_texture;
    sf::Sprite mass;
    float x_position;
    float y_position;
    float mass_sprite_x_size;
    float mass_sprite_y_size;
    float vertical_offset = 0;

    Mass() {
        string path_components[50] = { "resources", "images", "anvil.png" };
        mass_texture.loadFromFile(path_builder(path_components));
        mass.setTexture(mass_texture);
        mass_sprite_x_size = mass.getGlobalBounds().width;
        mass_sprite_y_size = mass.getGlobalBounds().height;
        y_position = -mass.getGlobalBounds().height; // start above the window!!!
    }

    int get_game_ceiling() {
        return y_position + mass.getGlobalBounds().height;
    }

    void compute(sf::RenderWindow& window) {
        float x_scale = window.getSize().y / mass_sprite_x_size;
        mass.setScale(x_scale, x_scale);

        y_position = -mass.getGlobalBounds().height + vertical_offset;

        x_position = (window.getSize().x - mass.getGlobalBounds().width) / 2;
        //vertical_offset += 0.1;//0.1; // this translates into space covered (down) onscreen.
    }

    void render(sf::RenderWindow& window) {
        mass.setPosition(x_position, y_position);
        window.draw(mass);
    }
};

class Ball {
public:
    float x, y;
    float radius;
    sf::Color color;
    float shape_x_velocity = 0;
    float shape_y_velocity = 0;
    sf::CircleShape shape;
    bool popped = false;

    Ball(
            float x,
            float y) : x(x), y(y) {

        color = pick_ball_color();
        shape.setFillColor(color);
        radius = BALL_RADIUS;
        shape.setRadius(BALL_RADIUS);
        shape.setOrigin(radius, radius); // centre of circle is its position
        shape.setPointCount(calculate_point_count(radius));
    }

    void set_velocity(float x_velocity, float y_velocity) {
        shape_x_velocity = x_velocity;
        shape_y_velocity = y_velocity;
    }

    void set_position(float new_x, float new_y) {
        x = new_x;
        y = new_y;
    }

    void compute(Mass& mass) {
        // check for a collision with the 'walls' of the game
        int game_y_minimum = mass.get_game_ceiling();
        int game_surface_max_x = mass.x_position + mass.mass.getGlobalBounds().width;
        int game_surface_min_x = mass.x_position;

        radius = BALL_RADIUS;
        shape.setRadius(BALL_RADIUS);

        if ((x - radius <= game_surface_min_x && shape_x_velocity < 0) || (x + radius >= game_surface_max_x && shape_x_velocity > 0)) {
            shape_x_velocity *= -1;
        }
        if ((y - radius <= game_y_minimum && shape_y_velocity < 0)) {
            shape_y_velocity = 0;
            shape_x_velocity = 0;
        }

        // Keep the ball inside the place it needs to be
        if (x - radius < game_surface_min_x) {
            x = radius + game_surface_min_x;
        }
        if (x + radius > game_surface_max_x) {
            x = game_surface_max_x - radius;
        }
        if (y - radius < game_y_minimum) {
            y = radius + game_y_minimum;
        }
        if (y > window_size[1]) {
            popped = true;
            y = -100;
            shape_x_velocity = 0;
            shape_y_velocity = 0;
        }

        x += shape_x_velocity;
        y += shape_y_velocity;
    }

    void collision(Ball& ball) {
        if (ball.popped == false) {
            // Calculate the distance between the two balls
            float distance = pythagorean_distance(x, y, ball.x, ball.y);
            if (distance < radius + ball.radius) {
                // Calculate the angle for the collision by assuming a tangent 
                // to each ball, then basic trig
                float angle = atan2(y - ball.y, x - ball.x);
                float sin_angle = sin(angle);
                float cos_angle = cos(angle);

                // Make sure that when the balls collide they are split apart so 
                // they don't stick together, or appear to be inside each other
                float overlap = 0.5f * (radius + ball.radius - distance + 1);
                x += cos_angle * overlap;
                y += sin_angle * overlap;
                ball.x -= cos_angle * overlap;
                ball.y -= sin_angle * overlap;

                // Remove their velocity so the balls stop moving.
                shape_x_velocity = 0;
                shape_y_velocity = 0;
                ball.shape_x_velocity = 0;
                ball.shape_y_velocity = 0;
            }
        }
    }

    void render(sf::RenderWindow& window) {
        shape.setPosition(x, y);
        if (popped == false) {
            window.draw(shape);
        }
    }
};

class Cannon {
public:
    sf::Texture cannon_texture;
    sf::Sprite cannon;
    float rotation = 0;
    int window_size[2] = { 0, 0 };
    int unrotated_cannon_height = 0;
    float cannon_x_position = 0;
    float cannon_y_position = 0;

    Cannon(vector<Ball>& game_balls) {
        string path_components[50] = { "resources", "images", "cannon.png" };
        cannon_texture.loadFromFile(path_builder(path_components));
        cannon.setTexture(cannon_texture);
        cannon.setPosition(500, 500);
        cannon.setScale(0.168, 0.168);

        load_cannon_with_ball(game_balls);
        load_cannon_with_ball(game_balls);
    }

    void set_position(float new_x, float new_y, vector<Ball>& game_balls) {
        cannon_x_position = new_x;
        cannon_y_position = new_y;

        cannon.setPosition(new_x, new_y);

        if (game_balls.size() >= 2) {
            for (int i = game_balls.size() - 2; i < game_balls.size(); ++i) {
                game_balls[i].set_position(new_x, new_y);
            }
        }
    }

    void load_cannon_with_ball(vector<Ball>& game_balls) {
        Ball game_ball = Ball(cannon.getPosition().x, cannon.getPosition().y);
        game_balls.emplace_back(game_ball);
    }

    void compute(sf::RenderWindow& window, PlayerInput& player_input, vector<Ball>& game_balls) {
        // define local variables
        int new_window_size[2] = { window.getSize().x, window.getSize().y };
        int cannon_width = cannon.getGlobalBounds().width;
        int cannon_height = cannon.getGlobalBounds().height;

        // mouse targeting!
        float opposite = player_input.get_mouse_position()[0] - cannon.getPosition().x;
        float adjacent = abs(player_input.get_mouse_position()[1] - cannon.getPosition().y);
        float new_rotation = 90 + atan(opposite / adjacent) * RADIANS_TO_DEGREES_CONVERSION_CONSTANT;

        // handle changes when display size changes
        if (window_size[0] != new_window_size[0] || window_size[1] != new_window_size[1]) {
            window_size[0] = new_window_size[0];
            window_size[1] = new_window_size[1];
            float window_scale = window_size[1] / 720.0;
            cannon.setScale(0.168 * window_scale, 0.168 * window_scale);
            cannon.setOrigin(366, 366);
            cannon.setRotation(0);
            unrotated_cannon_height = cannon.getGlobalBounds().height;
        }

        // update cannon translation and rotation
        if (new_rotation < 170 && new_rotation > 10) {
            rotation = new_rotation;
            cannon.setRotation(rotation);
        }

        set_position(window_size[0] / 2.0, window_size[1] - unrotated_cannon_height / 2, game_balls);
    }

    float get_rotation() {
        return rotation;
    }

    void render(sf::RenderWindow& window) {
        window.draw(cannon);
    }
};

class Text {
public:
    sf::Font font;
    sf::Text text;
    string text_attributes[5] = {};
    string font_face = FONT_PLAY;
    int position[2] = { 0, 0 };

    Text(string font_face=FONT_PLAY) {
        set_font_face(font_face);
    }

    void set_font_face(string new_font_face = FONT_PLAY) {
        // used to set the font for the button
        font_face = new_font_face;
        if (font_face == FONT_PLAY) {
            string path_components[50] = {
                "resources",
                "fonts",
                "Lilita_One Play",
                "Play-Regular.ttf" };

            font.loadFromFile(path_builder(path_components));
            text.setFont(font);
        }
        else if (font_face == FONT_REGULAR) {
            string path_components[50] = { "resources",
                "fonts",
                "Lilita_One",
                "LilitaOne-Regular.ttf" };

            font.loadFromFile(path_builder(path_components));
            text.setFont(font);
        }
        else {
            cout << "This is not a known font!";
        }
    }

    sf::Text get_text() {
        // used to get the text for button integration
        return text;
    }

    void set_position(
            sf::RenderWindow& window,
            int x_position,
            int y_position) {

        // sets the button position on screen. This is separate from render so 
        // it can be called without rendering the text

        if (x_position < 0) { // centre in x
            x_position = -(x_position - 1) + (window.getSize().x - text.getGlobalBounds().width) / 2;
        }

        if (y_position < 0) { // centre in y
            y_position = -(y_position - 1) + (window.getSize().y - text.getGlobalBounds().height) / 2;
        }

        position[0] = x_position;
        position[1] = y_position;
    }

    void render(
            sf::RenderWindow& window,
            string text_content,
            int text_size,
            sf::Color text_fill_color,
            bool text_is_bold = false,
            bool text_is_underlined = false) {

        // used to draw the text on screen efficiently
        bool text_changed = false;
        if (text_content != text_attributes[0]) {
            text_changed = true;
            text_attributes[0] = text_content;
        }
        if (to_string(text_size) != text_attributes[1]) {
            text_changed = true;
            text_attributes[1] = to_string(text_size);
        }
        if (to_string(text_fill_color.toInteger()) != text_attributes[2]) {
            text_changed = true;
            text_attributes[2] = to_string(text_fill_color.toInteger());
        }
        if (to_string(text_is_bold) != text_attributes[3]) {
            text_changed = true;
            text_attributes[3] = to_string(text_is_bold);
        }
        if (to_string(text_is_underlined) != text_attributes[4]) {
            text_changed = true;
            text_attributes[4] = to_string(text_is_underlined);
        }

        if (text_changed) {
            text.setString(text_content);
            text.setCharacterSize(text_size);
            text.setFillColor(text_fill_color);
            text.setStyle(sf::Text::Regular);
            if (text_is_bold) {
                text.setStyle(sf::Text::Bold);
            }
            if (text_is_underlined) {
                text.setStyle(sf::Text::Underlined);
            }
        }
        text.setPosition(position[0], position[1]);
        window.draw(text);
    }

};

class Button {
public:
    Text content = Text();
    sf::RectangleShape background;
    string font_face = FONT_PLAY;
    bool hovering = false;

    Button(string font_face=FONT_PLAY) {
        content.set_font_face(font_face);
        background.setOutlineColor(sf::Color::Black);
        background.setOutlineThickness(1);
    }

    void render(
            sf::RenderWindow& window, 
            int button_x_position,
            int button_y_position,
            string button_content,
            int button_text_size,
            sf::Color button_color,
            int button_padding=50) {

        // Used to actually create the button on-screen every frame
        content.render(
            window,
            button_content,
            button_text_size,
            sf::Color::Black); // get text dimensions

        // get the button size with the rect as a bounding box
        int button_x_size = content.get_text().getLocalBounds().width + button_padding;
        int button_y_size = content.get_text().getLocalBounds().height + button_padding;
        background.setSize(sf::Vector2f(button_x_size, button_y_size));

        // if the button position is negative, I can use this as a way of indicating a central position
        if (button_x_position < 0) { // centre in x
            button_x_position = -(button_x_position - 1) + (window.getSize().x - button_x_size) / 2;
        }

        if (button_y_position < 0) { // centre in y
            button_y_position = -(button_y_position - 1) + (window.getSize().y - button_y_size) / 2;
        }

        // set-up button background - so the player knows where to hit
        background.setPosition(button_x_position, button_y_position);
        background.setFillColor(button_color);

        // set the text position centred in the button
        content.set_position(
            window,
            button_x_position + button_padding/2,
            button_y_position + button_padding/2 - content.get_text().getLocalBounds().top);

        // draw the button rectangle to the window
        window.draw(background);

        // draw the text inside the button at the position we specified earlier.
        // Remember to think about layering so needs to be done last to appear first
        content.render(
            window,
            button_content,
            button_text_size,
            sf::Color::Black,
            false,
            hovering);
    }

    bool compute(PlayerInput& player_input) {
        // Used to actually detect collisions and mouse inputs with the button
        hovering = false;
        int button_position[2] = {
            player_input.get_mouse_position()[0],
            player_input.get_mouse_position()[1] };

        if (button_position[0] > background.getGlobalBounds().getPosition().x &&
                button_position[0] < background.getGlobalBounds().getPosition().x + background.getGlobalBounds().width) {

            if (button_position[1] > background.getGlobalBounds().getPosition().y &&
                    button_position[1] < background.getGlobalBounds().getPosition().y + background.getGlobalBounds().height) {
                hovering = true;
                if (player_input.get_player_button_input()) {
                    return true;
                }
            }
        }
        return false;
    }
};

class MainMenu {
    string game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    Text title_text = Text(FONT_REGULAR);
    Button play_button = Button();
    Button quit_button = Button();

public:
    MainMenu() {
    }

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
        // this runs once every frame
        // top to bottom rendering here where layering doesn't matter
        // render game title
        title_text.set_position(window, -1, 0);
        title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);

        // render play button
        bool play_button_result;
        play_button_result = play_button.compute(player_input);
        play_button.render(window, -1, -1, "Play!", 24, sf::Color::Red, 30);

        // render quit button
        bool quit_button_result;
        quit_button_result = quit_button.compute(player_input);
        quit_button.render(window, -1, -101, "quit", 24, sf::Color::Red, 20);

        // identify what menu to transition to next
        if (quit_button_result) {
            return QUIT;
        }
        else if (play_button_result) {
            return LEVEL_ONE;
        }
        // by default return to this menu on next run
        return MAIN_MENU;
    }

};

class LevelOne {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object = Cannon(game_balls);
    Mass mass_object = Mass();
    bool level_lost = false;
public:
    LevelOne(sf::RenderWindow& window) {
        mass_object.compute(window);
        int columns = 14;
        int x_pos = 0;
        int y_pos = 0;
        for (int row = 0; row < 6; row++) {
            if (row % 2 == 0) {
                columns = 14;
                x_pos = 0;
            }
            else {
                columns = 13;
                x_pos = BALL_RADIUS;
            }
            for (int column = 0; column < columns; column++) {
                Ball game_ball = Ball(x_pos+mass_object.x_position, y_pos);
                cout << x_pos + mass_object.x_position << " " << y_pos << endl;
                game_balls.emplace_back(game_ball);
                x_pos += BALL_RADIUS * 2;
            }
            y_pos += BALL_RADIUS * 2;
        }
    }

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
        // this runs once every frame
        float angle = 0;
        bool ball_in_motion = false;

        cannon_object.render(window);
        cannon_object.compute(window, player_input, game_balls);

        vector<int> garbage_ball_elements = {};

        int index = 0;
        for (auto& game_ball : game_balls) {
            if (index == game_balls.size() - 2) {
                angle = (cannon_object.get_rotation() - 90) * DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

                float x_pos = -sin(angle) * game_ball.radius / 2;
                float y_pos = -cos(angle) * game_ball.radius / 2;
                game_ball.set_position(cannon_object.cannon.getPosition().x+x_pos, cannon_object.cannon.getPosition().y-y_pos);
            }

            game_ball.compute(mass_object);
            game_ball.render(window);
            if (game_ball.popped) {
                garbage_ball_elements.emplace_back(index);
            }
            else if (game_ball.shape_x_velocity == 0 && game_ball.shape_y_velocity == 0 && index < game_balls.size() - 2) {
                for (auto& collision_game_ball : game_balls) {
                    game_ball.collision(collision_game_ball);
                }
            }
            index++;
        }

        game_balls[game_balls.size() - 1].render(window);

        index = 0;
        for (auto& game_ball : game_balls) {
            if (game_ball.shape_x_velocity != 0 || game_ball.shape_y_velocity != 0) {
                ball_in_motion = true;
            }
            if (game_ball.shape_x_velocity == 0 && game_ball.shape_y_velocity == 0 && index < game_balls.size()-2) {
                if (game_ball.shape.getPosition().y + game_ball.radius >= cannon_object.cannon_y_position - cannon_object.cannon.getGlobalBounds().height / 2) {
                    level_lost = true;
                }
            }
            index++;
        }

        if (mass_object.y_position >= cannon_object.cannon_y_position - cannon_object.cannon.getGlobalBounds().height / 2) {
            level_lost = true;
        }

        for (int& garbage_ball : garbage_ball_elements) {
            game_balls.erase(game_balls.begin() + garbage_ball);
        }

        mass_object.render(window);
        mass_object.compute(window);

        if (player_input.get_player_button_input() && ball_in_motion == false) {
            swap(game_balls[game_balls.size() - 2], game_balls[game_balls.size() - 1]);

            // get the angle the cannon is pointing at and fire the ball in that same direction
            angle = (cannon_object.get_rotation() - 90) * DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

            float x_velocity = sin(angle) * BALL_SPEED;
            float y_velocity = -cos(angle) * BALL_SPEED;

            game_balls[game_balls.size() - 2].set_velocity(x_velocity, y_velocity);
            //game_balls[game_balls.size() - 2].set_position(0, 0);

            cannon_object.load_cannon_with_ball(game_balls);
        }

        if (level_lost) {
            game_end_state = LOST;
            return END_MENU;
        }

        return LEVEL_ONE;
    }

    void reset_level() {
    }

};

class LevelTwo {
    vector<Ball> game_balls;
    Cannon cannon_object = Cannon(game_balls);
    Mass mass_object = Mass();
    bool level_lost = false;
public:
    LevelTwo(sf::RenderWindow& window) {
    }

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
        // this runs once every frame
        cannon_object.render(window);
        cannon_object.compute(window, player_input, game_balls);

        mass_object.render(window);
        mass_object.compute(window);

        if (level_lost) {
            game_end_state = LOST;
            return END_MENU;
        }

        return LEVEL_TWO;
    }

    void reset_level() {
    }
};

class GameEndMenu {
    Text title_text = Text(FONT_REGULAR);
    Text game_win_state = Text(FONT_PLAY);

    Button play_again_button = Button();
    Button quit_button = Button();
public:
    GameEndMenu() {
    }

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input, bool game_won) {
        // this runs once every frame
        // render game title
        title_text.set_position(window, -1, 0);
        title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);

        // render game win state (win/loose)
        game_win_state.set_position(window, -1, 200);

        string game_win_state_text_content;
        sf::Color game_win_state_text_color;
        if (game_won) {
            game_win_state_text_content = "You WIN!!!";
            game_win_state_text_color = sf::Color::Green;
        }
        else {
            game_win_state_text_content = "You Lost!!!";
            game_win_state_text_color = sf::Color::Red;
        }

        game_win_state.render(window, game_win_state_text_content, 50, game_win_state_text_color, true);

        // render play again button
        bool play_again_button_result;
        play_again_button_result = play_again_button.compute(player_input);
        play_again_button.render(window, -1, -1, "Play Again!", 24, sf::Color::Red, 30);

        // render quit button
        bool quit_button_result;
        quit_button_result = quit_button.compute(player_input);
        quit_button.render(window, -1, -101, "quit", 24, sf::Color::Red, 20);

        // identify what menu to transition to next
        if (quit_button_result) {
            return QUIT;
        }
        else if (play_again_button_result) {
            return MAIN_MENU;
        }

        // by default return to this menu on next run
        return END_MENU;
    }
};

class PauseMenu {
public:
    PauseMenu() {
    }

    string run_menu(
            sf::RenderWindow& window,
            string* menu_navigation,
            PlayerInput& player_input) {

        // this runs once every frame
        return menu_navigation[1]; // go to previous level
        return PAUSE_MENU; // stay on current window alternate between 
        // these for menu navigation
    }
};

int main()
{
    srand(time(0)); // enforces random values using seed of current time.

    sf::RenderWindow window;
    window.create(
        sf::VideoMode(window_size[0], window_size[1]),
        "Bouncing Balls!");

    window.setVerticalSyncEnabled(true); // v-sync is our frame limiter here.
    // No need for clocks!

    // set-up local variables
    string menu_navigation[] = { MAIN_MENU, MAIN_MENU };
    /*
    * The Menu Navigation System, created in the variable menu_navigation, allows us
    * to easily move between the current menu and the previous menu - so that the
    * player can return to the main menu at any time or a pause menu and the game can
    * "remember" what was happening before this switch occurred. In a larger game
    * the array could be larger as there are more menus to navigate through,
    * but I am keeping this relatively simple and small.
    */
    int score = 0;
    bool game_won = false;
    bool window_full_screen = false;
    bool game_running = true;

    string next_component;

    // set-up game componentry
    PlayerInput player_input = PlayerInput();

    // set-up game levels
    MainMenu main_menu = MainMenu();
    LevelOne level_one = LevelOne(window);
    LevelTwo level_two = LevelTwo(window);
    GameEndMenu end_menu = GameEndMenu();
    PauseMenu pause_menu = PauseMenu();

    // Load pop sounds. Using music here means files aren't actually loaded
    // and instead streamed as needed, meaning this isn't actually that
    // inefficient!
    sf::Music pop_sounds[15];
    for (int i = 0; i < 15; i++) { // 16
        string file = to_string(i + 1) + ".wav";
        string path_components[50] = { "resources", "sounds", "pops", file };
        string file_path = path_builder(path_components);
        pop_sounds[i].openFromFile(file_path);
    }

    play_random_pop_sounds(pop_sounds, rand() % 15); // temporary!!!

    sf::Music main_theme;
    string path_components[50] = { "resources", "music", "main theme [extended].ogg"};
    main_theme.openFromFile(path_builder(path_components));
    main_theme.setLoop(true);
    main_theme.setVolume(15);
    //main_theme.play();

    while (game_running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                game_running = false;
                break;
            }
            else if (event.type == sf::Event::Resized) // https://stackoverflow.com/questions/38466518/sfml-center-view-after-window-resize#:~:text=Just%20use%20another%20constructor%20and%20pass%20the%20center,window%20and%20keep%20the%20center%20window.setView%28sf%3A%3AView%28window.getView%28%29.getCenter%28%29%2C%20sf%3A%3AVector2f%28%28float%29event.size.width%2C%20%28float%29event.size.height%29%29%29%3B - 21/10/24 @ 16:59
            {
                // The window was resized, update the view accordingly
                sf::Vector2u newSize = window.getSize();
                sf::FloatRect window_dimensions_rectangle(0, 0, newSize.x, newSize.y);
                sf::View viewport = sf::View(window_dimensions_rectangle);
                window.setView(viewport);
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Enter:
                    player_input.set_player_button_input(true);
                    break;
                case sf::Keyboard::Escape:
                    menu_navigation[1], menu_navigation[0] = menu_navigation[0], MAIN_MENU;
                    // cheeky element switch
                    break;
                case sf::Keyboard::F11:
                    window_full_screen = !window_full_screen;
                    if (window_full_screen) {
                        // create window (do last so not unresponsive whilst the game loads)
                        window.create(
                            sf::VideoMode::getDesktopMode(),
                            "Bouncing Balls!",
                            sf::Style::Fullscreen);
                    }
                    else {
                        // create window (do last so not unresponsive whilst the game loads)
                        window.create(
                            sf::VideoMode(window_size[0], window_size[1]),
                            "Bouncing Balls!");
                    }

                    window.setVerticalSyncEnabled(true); // v-sync is our frame limiter here.
                    // No need for clocks!

                    break;
                case sf::Keyboard::Num1: // [temporary] forced switching between levels
                    menu_navigation[0] = MAIN_MENU;
                    menu_navigation[1] = MAIN_MENU;
                    break;
                case sf::Keyboard::Num2: // [temporary] forced switching between levels
                    menu_navigation[0] = LEVEL_ONE;
                    menu_navigation[1] = LEVEL_ONE;
                    break;
                case sf::Keyboard::Num3: // [temporary] forced switching between levels
                    menu_navigation[0] = LEVEL_TWO;
                    menu_navigation[1] = LEVEL_TWO;
                    break;
                case sf::Keyboard::Num4: // [temporary] forced switching between levels
                    menu_navigation[0] = END_MENU;
                    menu_navigation[1] = END_MENU;
                    break;
                case sf::Keyboard::Num5: // [temporary] forced switching between levels
                    menu_navigation[0] = PAUSE_MENU;
                    menu_navigation[1] = PAUSE_MENU;
                    break;
                }
            }
        }

        window.clear(sf::Color(255, 255, 255));

        if (menu_navigation[0] == MAIN_MENU) {
            next_component = main_menu.run_menu(window, player_input);
            if (next_component != MAIN_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = MAIN_MENU;
            }
        }
        else if (menu_navigation[0] == LEVEL_ONE) {
            next_component = level_one.run_menu(window, player_input);
            if (next_component != LEVEL_ONE) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = LEVEL_ONE;
                if (next_component != PAUSE_MENU) {
                    level_two.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == LEVEL_TWO) {
            next_component = level_two.run_menu(window, player_input);
            if (next_component != LEVEL_ONE) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = LEVEL_TWO;
                if (next_component != PAUSE_MENU) {
                    level_two.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == END_MENU) {
            next_component = end_menu.run_menu(
                window,
                player_input,
                game_won);

            if (next_component != END_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = END_MENU;
            }
        }
        else if (menu_navigation[0] == PAUSE_MENU) {
            next_component = pause_menu.run_menu(
                window,
                menu_navigation,
                player_input);

            if (next_component != PAUSE_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = PAUSE_MENU;
            }
        }
        else if (menu_navigation[0] == QUIT) {
            break;
        }
        else {
            cout << "Current level not recognised!!!";
            menu_navigation[0] = MAIN_MENU;
            menu_navigation[1] = MAIN_MENU;
        }

        window.display();
        player_input.update(window);
    }

    return 0;
}