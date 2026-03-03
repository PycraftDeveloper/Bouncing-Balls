/*
This program is used to create the basic components needed in most games and general applications.
This is the header file, and the functionality behind this file can be found in 'UtilityObjects.cpp'.
*/
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Constants.h"
#include "Registry.h"

class PlayerInput {
    // This class handles and controls how the user interacts with the game. In this game, the mouse
    // left button and keyboard enter key both end up having the same behaviour. So instead of continuing
    // to treat them as separate inputs, they can be combined together to simply the game logic.
    // This input method has no effect on the 'Esc' key that can also be used as a different input for the game.
private:
    int mouse_position[2] = { 0, 0 }; // stores the mouse position for the frame, relative to the
    // top left corner of the game window.
    sf::Mouse mouse; // used to interact with the SFML mouse object.
    bool player_button_input = false; // this flag is used to determine if the player is currently using either of the
    // available inputs.
    bool player_continual_interation = false; // this flag was added later, and allows for the game to know when the user
    // has interacted with an element in the game, and then determine if the next interaction should go ahead. This is used
    // to prevent the bug of appearing to be able to click two buttons placed in the same position between two menus connected
    // together.

public:
    PlayerInput();

    void update(sf::RenderWindow& window); // this is called at the end of each game loop, and will update where the game thinks
    // the mouse cursor should be for the next frame.

    int* get_mouse_position(); // this is used by the game to get the position of the mouse cursor for that frame.

    bool get_player_button_input(); // this is used to determine if the player is attempting to activate a clickable
    // object, including buttons and the cannon.

    void set_player_button_input(bool value); // this is used to set manually when the user is interacting with the game
    // from the event loop, as the mouse button interactions are otherwise handled in the 'update' method.
};

class Text {
    // This class streamlines the process of text rendering by creating an easier to use interface, and automatically
    // resolving font paths based on the name of the font to use.
private:
    bool loaded = false; // This determines if the font file is loaded into RAM, which allows font that isn't being
    // used by the game to be deleted from memory.
    string file_path; // This stores the path of the font.
    sf::Font font;
    sf::Text text;
    string text_attributes[5] = {}; // stores the customisation of the text previously rendered. Then only when the
    // text changes in some way is it updated, improving the efficiency of rendering text.
    string font_face = Constants::FONT_PLAY; // stores the font face to use, which can be one of two pre-defined constants.
    // this is then used in the calculation of the file path, which can be then placed in the file_path string.
    int position[2] = { 0, 0 }; // stores the position of the text on-screen.
    int rotation = 0; // stores the rotation of the text, allowing for custom orientations.

public:
    Text(string font_face = Constants::FONT_PLAY);

    void set_font_face(string new_font_face = Constants::FONT_PLAY); // allows the font face to be set after the font has been created.
    // This allows the font to be defined in an array, then customized later on.

    sf::Text get_text(); // This is used to get the rendered text, which can then be used for further customisation and (primarily)
    // for calculations of rotation and position by interacting with the SFML text directly.

    void set_position(
        sf::RenderWindow& window,
        int x_position,
        int y_position); // sets the position of the text on screen. If either position value is -1, then the text is centred and
        // using -x or -y allows for text to be offset from this central position.

    void set_rotation(int angle); // sets the rotation of the text.

    void render(
        sf::RenderWindow& window,
        string text_content,
        int text_size,
        sf::Color text_fill_color,
        bool text_is_bold = false,
        bool text_is_underlined = false); // renders a string to a text object using the font defined earlier. The possible formatting
        // options are bold and underlined, and the text size and fill colour can also be specified. These, along with the text content
        // are all stored in the text attributes array for caching later.

    void load(); // loads the font from the disk using the file_path specified earlier into RAM and associates it with the text SFML object.

    void unload(); // removes any association to the font object, before removing it from RAM to free up space.
};

class Button {
    // This class is an extension of sorts from the text rendering class above, taking the rendered text and using its position and dimensions
    // to create a hit-box allowing the user to interact with it.
private:
    Text content; // Here the simplified rendering pipeline of the custom text rendering solution is used as opposed to the native SFML solution.
    sf::RectangleShape background; // This defines the background of the button, forming its hit-box.
    string font_face = Constants::FONT_PLAY; // used to specify the font face for the text. By default it will use the PLAY variant.
    bool hovering = false; // Used to determine if the button currently has the mouse within its hit-box, which can be used to apply some
    // graphical changes so the button appears to 'acknowledge' the presence of the mouse cursor.

public:
    Button(string font_face = Constants::FONT_PLAY);

    void render(
        sf::RenderWindow& window,
        int button_x_position,
        int button_y_position,
        string button_content); // This is the rendering procedure, and handles positioning the button, as well as the rendering of the text
        // but using a simplified interface as not all the options defined for the text class above are necessary here.

    bool compute(PlayerInput& player_input); // called once per frame, this is used to update the state of the button, and allows the button to
    // change stylistically depending on how the user interacts with it. This is also where button activation by clicking or pressing enter whilst
    // hovering gets triggered.

    void unload(); // this is used not by the button its self, but as a way to unload the text object that it uses. The button object its self
    // has no elements loaded from the disk, so none with a size significant enough to warrant deleting to free up space.

    void load(); // this is again not used by the button, but passes on the load instruction to the text component for reasons similar to the procedure above.
};

class MediaControls {
    // This class is used to create the media controls seen in the main menu, pause menu and game end menu that allow you to turn on/off
    // game music and sounds separately. Both of these buttons are rendered as one here, with only one texture being used for each the music and sound
    // options. This reduces memory wastage by loading all 4 textures at once. This was initially planned to be two different widgets, or two instances of
    // the same widget. However this idea was later scrapped as they both appear in a pair on screen, and it was much easier to place them about a central desired
    // target than to calculate them individually. It should be noted that this widget renders two buttons, but these are NOT the same buttons as seen in the Button class
    // as these buttons use textures instead of font/text to display content.
private:
    sf::Texture music_button_texture;
    sf::Sprite music_button;

    sf::Texture sound_button_texture;
    sf::Sprite sound_button;
    // here, the textures and sprites are defined for each of the two buttons this widget renders.

    string music_on_button_texture_path;
    string music_off_button_texture_path;

    string sound_on_button_texture_path;
    string sound_off_button_texture_path;
    // Despite only having two places for textures, all four paths are stored by the class, so that the textures can be swapped
    // out as needed.

    bool loaded = false;// This is used to determine if the button textures exist yet in RAM.
    bool conditions_changed = false; // This is used to determine if the button textures that exist in RAM need to be refreshed.

    int music_position[2] = { 0, 0 };
    int sound_position[2] = { 0, 0 };
    // These store the positions for each of the two buttons used in this window, in the form (x, y).

public:
    MediaControls();

    void compute(PlayerInput& player_input); // This method handles user interaction, as well as completely managing the input events, unlike the button class
    // which returns a value to be interpreted by the caller. This method therefore also handles the changes to the Registry settings allowing this widget to function.

    void render(sf::RenderWindow& window, int x_position, int y_position); // This method is used to render the widgets on screen, and more importantly, also controls where
    // about each of the two buttons should be placed, relative to a specified central position in the window.

    void load(); // This is used to load both the textures used by the buttons. This also controls adjusting the size of the widgets so they fit properly into the window.
    // This method is also used to strategically reload textures when the relevant registry states are changed.

    void unload(); // This is used to unload both button textures, to reduce this widgets size in memory.
};