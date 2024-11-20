#include <SFML/Graphics.hpp>

#include "UtilityObjects.h"
#include "Constants.h"
#include "Registry.h"

using namespace std;

PlayerInput::PlayerInput() {
    sf::Mouse mouse();
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