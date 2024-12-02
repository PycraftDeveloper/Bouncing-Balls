#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Constants.h"
#include "Registry.h"

class PlayerInput {
private:
    int mouse_position[2] = { 0, 0 };
    sf::Mouse mouse;
    bool player_button_input = false;
    bool player_continual_interation = false;

public:
    PlayerInput();

    void update(sf::RenderWindow& window);

    int* get_mouse_position();

    bool get_player_button_input();

    void set_player_button_input(bool value);
};

class Text {
public:
    sf::Font font;
    sf::Text text;
    string text_attributes[5] = {};
    string font_face = Constants::FONT_PLAY;
    int position[2] = { 0, 0 };
    int rotation = 0;

    Text(string font_face = Constants::FONT_PLAY);

    void set_font_face(string new_font_face = Constants::FONT_PLAY);

    sf::Text get_text();

    void set_position(
        sf::RenderWindow& window,
        int x_position,
        int y_position);

    void set_rotation(int angle);

    void render(
        sf::RenderWindow& window,
        string text_content,
        int text_size,
        sf::Color text_fill_color,
        bool text_is_bold = false,
        bool text_is_underlined = false);
};

class Button {
public:
    Text content = Text();
    sf::RectangleShape background;
    string font_face = Constants::FONT_PLAY;
    bool hovering = false;

    Button(string font_face = Constants::FONT_PLAY);

    void render(
        sf::RenderWindow& window,
        int button_x_position,
        int button_y_position,
        string button_content,
        int button_text_size,
        int button_padding = 50);

    bool compute(PlayerInput& player_input);
};

class Sound {
private:
    sf::Music sound;
    string sound_file;
    bool loaded = false;
public:
    Sound();

    void init(string file_path);

    void load();

    void unload();

    void play();

    void stop();

    void set_volume(int volume);
};