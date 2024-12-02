#include <SFML/Graphics.hpp>
#include <iostream>

#include "UtilityObjects.h"
#include "UtilityFunctions.h"

using namespace std;

PlayerInput::PlayerInput() {
}

void PlayerInput::update(sf::RenderWindow& window) {
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

int* PlayerInput::get_mouse_position() {
    return mouse_position;
}

bool PlayerInput::get_player_button_input() {
    return player_button_input;
}

void PlayerInput::set_player_button_input(bool value) {
    player_button_input = value;
}

Text::Text(string font_face) {
    set_font_face(font_face);
}

void Text::set_font_face(string new_font_face) {
    // used to set the font for the button
    font_face = new_font_face;
    if (font_face == Constants::FONT_PLAY) {
        string path_components[4] = {
            "resources",
            "fonts",
            "Lilita_One Play",
            "Play-Regular.ttf" };

        font.loadFromFile(path_builder(path_components));
        text.setFont(font);
    }
    else if (font_face == Constants::FONT_REGULAR) {
        string path_components[4] = { "resources",
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

sf::Text Text::get_text() {
    // used to get the text for button integration
    return text;
}

void Text::set_position(
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

void Text::set_rotation(int angle) {
    rotation = angle;
}

void Text::render(
    sf::RenderWindow& window,
    string text_content,
    int text_size,
    sf::Color text_fill_color,
    bool text_is_bold,
    bool text_is_underlined) {

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
    text.setRotation(rotation);
    window.draw(text);
}

Button::Button(string font_face) {
    content.set_font_face(font_face);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(1);
}

void Button::render(
    sf::RenderWindow& window,
    int button_x_position,
    int button_y_position,
    string button_content,
    int button_text_size,
    int button_padding) {

    sf::Color button_color = sf::Color(33, 115, 204);

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
        button_x_position + button_padding / 2,
        button_y_position + button_padding / 2 - content.get_text().getLocalBounds().top);

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

bool Button::compute(PlayerInput& player_input) {
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

Sound::Sound() {
}

void Sound::init(string file_path) {
    sound.openFromFile(file_path);
}

void Sound::play() {
    sound.stop();
    sound.play();
}

void Sound::stop() {
    sound.stop();
}

void Sound::set_volume(int volume) {
    sound.setVolume(volume);
}