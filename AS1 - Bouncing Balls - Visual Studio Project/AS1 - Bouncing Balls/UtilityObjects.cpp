/*
This program is used to create the basic components needed in most games and general applications.
This is the program file, for the objects outlined in the header file 'UtilityObjects.h'.
*/
#include <SFML/Graphics.hpp>
#include <iostream>

#include "UtilityObjects.h"
#include "UtilityFunctions.h"

using namespace std;

PlayerInput::PlayerInput() {
}

void PlayerInput::update(sf::RenderWindow& window) {
    if (window.hasFocus() == false) {
        player_button_input = false;
    }
    else {
        sf::Vector2i internal_position = mouse.getPosition(
            window);
        mouse_position[0] = internal_position.x;
        mouse_position[1] = internal_position.y;
        player_button_input = mouse.isButtonPressed(
            sf::Mouse::Button::Left);
        // When the window is in focus, then and only then will the mouse position and player button input be
        // calculated, this is to prevent the user from accidentally interacting with the game whilst intending to
        // interact with something else on the system.
    }
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
    font_face = new_font_face;
    string path_components[4];

    if (font_face == Constants::FONT_PLAY) {
        path_components[0] = "resources";
        path_components[1] = "fonts";
        path_components[2] = "Play";
        path_components[3] = "Play-Regular.ttf";
    }
    else if (font_face == Constants::FONT_REGULAR) {
        path_components[0] = "resources";
        path_components[1] = "fonts";
        path_components[2] = "Lilita_One";
        path_components[3] = "LilitaOne-Regular.ttf";
    }
    else {
        cout << "This is not a known font!";
        return;
    }
    file_path = path_builder(path_components);

    // The constants representing the two fonts the game uses can be passed into this function
    // and from this a path can be constructed. It should be noted that whilst the path could be directly
    // associated with the constant, the approach seen here allows for easier cross platform changes by using the
    // 'path_builder' function.
}

sf::Text Text::get_text() {
    // used to get the text for button integration
    return text;
}

void Text::set_position(
    sf::RenderWindow& window,
    int x_position,
    int y_position) {
    if (loaded == false) {
        // if the font isn't yet loaded, then it will be used in this function, so will need to be loaded
        // immediately.
        load();
    }

    // sets the button position on screen. This is separate from render so 
    // it can be called without rendering the text primarily this is used in the button mechanic.

    if (x_position < 0) { // centre in x, and also handle the negative input behaviour
        x_position = -(x_position - 1) + (window.getSize().x - text.getGlobalBounds().width) / 2;
    }

    if (y_position < 0) { // centre in y, and also handle the negative input behaviour
        y_position = -(y_position - 1) + (window.getSize().y - text.getGlobalBounds().height) / 2;
    }

    // The negative input behaviour: When -1 is entered, the text is centred in that axis. If a larger negative value is
    // entered, for example -10, then the position will be 10 pixels from the centre of the screen in that axis.

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
    if (loaded ==false) {
        // if the font isn't yet loaded, then it will be used in this function, so will need to be loaded
        // immediately.
        load();
    }

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
        // only when the text changes do we bother to recalculate it's data.
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

void Text::load() {
    // here the font identified earlier is loaded from the file, then associated with the text object.
    // only once this has been done is the font considered loaded. This is to prevent the font from being used before
    // it is ready should there be any parallel processing going on.
    font.loadFromFile(file_path);
    text.setFont(font);
    loaded = true;
}

void Text::unload() {
    // The font is immediately considered as unloaded, ensuring that any parallel rendering doesn't break by trying to render with
    // the font whilst it is being removed from memory. Only after the flag has been set is the new font object created (which doesn't
    // have a font file loaded, reducing its size in memory). This is then assigned as the new font for the text object, and then overwrites the
    // existing font object. This can only be done once all references to the original font object have been destroyed.
    loaded = false;
    sf::Font new_font;
    text.setFont(new_font);
    font = new_font;
}

Button::Button(string font_face) {
    content.set_font_face(font_face);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(1);
    // Here, the button is immediately assigned a font face type, as well as some initial values for customizing the style of the button. Because all
    // the buttons in the level will be themed similarly, there is no need for these customizations to be exposed to the rest of the game.
}

void Button::render(
    sf::RenderWindow& window,
    int button_x_position,
    int button_y_position,
    string button_content) {

    sf::Color button_color = sf::Color(33, 115, 204); // Sets the button background colour to a darker version of the background colour. This is done to
    // ensure the button has a clearly defined background, whilst also ensuring the button is easy to read.

    // Used to actually create the button on-screen every frame
    content.render(
        window,
        button_content,
        24,
        sf::Color::Black);
        
    // get text dimensions
    // get the button size with the rect as a bounding box
    int button_x_size = 400;
    int button_y_size = 50;
    background.setSize(sf::Vector2f(button_x_size, button_y_size));

    if (button_x_position < 0) {// centre in x, and also handle the negative input behaviour
        button_x_position = -(button_x_position - 1) + (window.getSize().x - button_x_size) / 2;
    }

    if (button_y_position < 0) {// centre in y, and also handle the negative input behaviour
        button_y_position = -(button_y_position - 1) + (window.getSize().y - button_y_size) / 2;
    }

    // set-up button background - so the player knows where to hit
    background.setPosition(button_x_position, button_y_position);
    background.setFillColor(button_color);

    // set the text position centred in the button
    content.set_position(
        window,
        button_x_position + (button_x_size - content.get_text().getGlobalBounds().width)/2,
        button_y_position + (button_y_size - content.get_text().getGlobalBounds().height)/2 - 5); // There is a slight offset here so that the text
        // is rendered in the right place in the y axis. I'm not too sure why its being rendered in slightly the wrong place.

    // draw the button rectangle to the window
    window.draw(background);

    // draw the text inside the button at the position we specified earlier.
    // Remember to think about layering so needs to be done last to appear first
    content.render(
        window,
        button_content,
        24,
        sf::Color::Black,
        false,
        hovering);
}

bool Button::compute(PlayerInput& player_input) {
    // Used to actually detect collisions and mouse inputs with the button
    hovering = false;
    int mouse_position[2] = {
        player_input.get_mouse_position()[0],
        player_input.get_mouse_position()[1] };

    if (mouse_position[0] > background.getGlobalBounds().getPosition().x &&
        mouse_position[0] < background.getGlobalBounds().getPosition().x + background.getGlobalBounds().width) {
            // check the mouse is first aligned in the x axis

        if (mouse_position[1] > background.getGlobalBounds().getPosition().y &&
            mouse_position[1] < background.getGlobalBounds().getPosition().y + background.getGlobalBounds().height) {
                // then check the mouse is aligned in the y axis before considering the mouse cursor to be inside the button hit-box.
            hovering = true; // when the cursor is inside the button hit-box, this flag is set to true, which will adjust how the button appears
            // when the render function is next called.
            if (player_input.get_player_button_input()) {
                return true; // if the button has been clicked, or the enter key pressed, then the button has been activated and this is sent back
                // to the caller to allow for a specific action to be performed.
            }
        }
    }
    return false;
}

void Button::load() {
    content.load(); // passes the load from the button into the text object, as this attribute wont otherwise be accessible outside the button.
}

void Button::unload() {
    content.unload(); // passes the unload from the button into the text object, as this attribute wont otherwise be accessible outside the button.
}

MediaControls::MediaControls() {
    string music_on_button_texture_path_components[4] = {
        "resources", "images", "music_on.png"
    };

    string music_off_button_texture_path_components[4] = {
        "resources", "images", "music_off.png"
    };

    string sound_on_button_texture_path_components[4] = {
        "resources", "images", "sound_on.png"
    };

    string sound_off_button_texture_path_components[4] = {
        "resources", "images", "sound_off.png"
    };
    // These four arrays store the folder/file names that can be constructed into a fully qualified path that points to the texture location on the system.

    music_on_button_texture_path = path_builder(music_on_button_texture_path_components);
    music_off_button_texture_path = path_builder(music_off_button_texture_path_components);

    sound_on_button_texture_path = path_builder(sound_on_button_texture_path_components);
    sound_off_button_texture_path = path_builder(sound_off_button_texture_path_components);
    // Here, all four paths are constructed and stored into the strings defined in the class, so that the right textures can be easily accessed in the load method.
}

void MediaControls::compute(PlayerInput& player_input) {
    if (conditions_changed) {
        conditions_changed = false;
        load();
    } // This is used to selectively reload the textures used for each button when their corresponding registry states are changed. - resetting the flag.

    int mouse_position[2] = {
        player_input.get_mouse_position()[0],
        player_input.get_mouse_position()[1] };
    // This is used to store the mouse's current position in a 2D array.

    // Music button & Mouse pointer collision detection - start
    if (mouse_position[0] > music_position[0] &&
        mouse_position[0] < music_position[0] + music_button.getGlobalBounds().width) {
        // check the mouse is first aligned in the x axis

        if (mouse_position[1] > music_position[1] &&
            mouse_position[1] < music_position[1] + music_button.getGlobalBounds().height) {

            // check the mouse is first aligned in the y axis
            if (player_input.get_player_button_input()) {
                // There is no hovering behaviour here - to reduce the game's overall size - so only when the user clicks the mouse or presses the
                // 'enter' key does anything happen.
                Registry::play_music = Registry::play_music == false;
                // The registry Boolean value is flipped.
                conditions_changed = true;
                // The button needs its texture changed, so this flag is set so it occurs in the next frame.
            }
        }
    }
    // Music button & Mouse pointer collision detection - end

    // Sound button & Mouse pointer collision detection - start
    if (mouse_position[0] > sound_position[0] &&
        mouse_position[0] < sound_position[0] + sound_button.getGlobalBounds().width) {

        if (mouse_position[1] > sound_position[1] &&
            mouse_position[1] < sound_position[1] + sound_button.getGlobalBounds().height) {

            if (player_input.get_player_button_input()) {
                Registry::play_sounds = Registry::play_sounds == false;
                conditions_changed = true;
            }
        }
    }
    // Sound button & Mouse pointer collision detection - end
}

void MediaControls::render(sf::RenderWindow& window, int x_position, int y_position) {
    if (loaded == false) {
        load();
    } // load the required textures if they do not already exist in RAM.

    // positioning equation breakdown (for negative integers ONLY):
    // 1. Position to centre of window.
    // 2. Apply abs offset.
    // 3. Apply widget specific customisation

    if (x_position < 0) {// centre in x, and also handle the negative input behaviour
        music_position[0] = -(x_position - 1) + (Registry::window_size[0] / 2) - music_button.getGlobalBounds().width - 25;
        sound_position[0] = -(x_position - 1) + (Registry::window_size[0] / 2) + 25;
    }
    else {
        music_position[0] = x_position - 25;
        music_position[0] = x_position + 25;
    }

    if (y_position < 0) {// centre in y, and also handle the negative input behaviour
        music_position[1] = -(y_position - 1) + (Registry::window_size[1] - music_button.getGlobalBounds().width) / 2;
        sound_position[1] = -(y_position - 1) + (Registry::window_size[1] - music_button.getGlobalBounds().width) / 2;
    }
    else {
        music_position[1] = y_position;
        sound_position[1] = y_position;
    }
    // In the button class, the values passed in for position where modified directly. Here this doesn't happen and array elements are updated to it is important to
    // ensure that the widget can be placed using non-negative values!

    // sets the position of each widget to the positions determined above! Remember, each button has its own position so they don't appear in the same place.
    music_button.setPosition(music_position[0], music_position[1]);
    sound_button.setPosition(sound_position[0], sound_position[1]);

    window.draw(music_button);
    window.draw(sound_button);
    // The buttons are drawn on-screen.
}

void MediaControls::load() {
    if (Registry::play_music) {
        music_button_texture.loadFromFile(music_on_button_texture_path);
    }
    else {
        music_button_texture.loadFromFile(music_off_button_texture_path);
    }
    music_button.setTexture(music_button_texture);
    // This loads the correct music texture into RAM by reading the registry state - in the same way that the music player does to ensure they both represent
    // the same thing correctly. Any existing value is overwritten to reduce memory wastage.

    if (Registry::play_sounds) {
        sound_button_texture.loadFromFile(sound_on_button_texture_path);
    }
    else {
        sound_button_texture.loadFromFile(sound_off_button_texture_path);
    }
    sound_button.setTexture(sound_button_texture);
    // This loads the correct sound texture into RAM by reading the same memory state as the one used in sound playback - this is identical in functionality to the
    // code immediately above.

    music_button.setScale(0.1, 0.1);
    sound_button.setScale(0.1, 0.1);
    // Resizes the widgets to fit correctly on-screen. Because the sprite's size has changed, there is no need to only apply this once, as the texture re-applied will
    // reset the sprite back to its original size.

    loaded = true;
}

void MediaControls::unload() {
    loaded = false;

    sf::Texture blank_texture;
    music_button.setTexture(blank_texture);
    sound_button.setTexture(blank_texture);

    music_button_texture = blank_texture;
    sound_button_texture = blank_texture;
    // This unloads both textures, using the same blank texture - after first having removed all association with the texture to ensure proper removal.
}