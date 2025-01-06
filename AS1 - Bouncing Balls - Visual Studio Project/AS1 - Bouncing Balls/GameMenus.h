/*
This is one of the most important programs in the bouncing balls code. Here each game menu is defined, from the main menu to the
game levels. Furthermore, this program also dictates how each level connected to each other - but not how they get there. It should
also be noted that a lot of the functionality behind this menu is defined in the other programs and the game mechanics are largely
defined within the game objects. This is just the header file and the actual code can be found in the file: 'GameMenus.cpp'
*/
#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"
#include "GameObjects.h"
#include "GameGraphics.h"

class MainMenu {
    // This is in charge of creating the main menu. This is the first menu the user sees, and is important in directing the user
    // to play the game.
private:
    Text title_text = Text(Constants::FONT_REGULAR); // used to display the name of the game front and centre on the main menu.
    Button play_button;
    Button quit_button;
    MainMenuInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;
    MediaControls media_controls;

public:
    MainMenu(); // Used to set-up the main menu and its components.

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input); // This is called once per frame if this is the current menu
    // and renders its content on top of the content seen in the background of the game. But before it renders anything, each object in the menu
    // needs to be computed. This will also trigger any resources needed to load if they haven't already.

    void unload(); // This is passed through to each of the menu's objects to ensure that any files opened into RAM are cleared when the menu
    // is no longer needed, reducing the menu's memory usage.
};

class LevelTemplate {
    // This actually isn't a level in the game. Instead, this is inherited by the game levels, as they both operate on a fundamentally
    // similar - but not identical - system. This class couldn't be used as a level on its own, as there are components missing from this
    // that are defined within each level that differentiates them from each other.
protected:
    vector<Ball> game_balls; // A dynamically sized array used to store all the ball instances in the level. It MUST be noted that the last two balls
    // will always belong to the cannon, and are largely considered separate from the rest, but exist still as a ball in the game, so must be treated as such.
    Cannon cannon_object;
    Mass mass_object;
    bool game_lost = false; // This is used to determine if the game has been lost or not, this is based on the state of the level. As failing the level
    // automatically means the game has also been lost, these two can be connected, and then if the player looses, this controls how the menu transitions to the
    // end menu, and how it should display.
    LevelInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;
    sf::Music pop_sounds[15]; // an array used to store all of the 15 unique pop-sound effects in the game, which are played to signify a ball has been popped by the player.
    // These sounds are streamed, so there is no need to load and unload them, as their size in RAM is very small.
    sf::Music cannon_fire_sound;
    sf::Music anvil_fail_sound;
    float level_end_time = -1; // This stores the time at which the level ended. Either because the player won or lost the game. This is important as it allows for a brief
    // pause before transitioning to the next menu, allowing the user to celebrate their victory, or realise they failed. -1 is considered to be a NULL value here, and will
    // be overridden by the game the moment the level is over.
    bool level_over = false; // This is used to determine if the level has ended or not. Here no differentiation if made between a win or loose condition as this variable is
    // largely in charge of managing the transition between menus.
    Text level_complete_text;
    Text score_text;

    LevelTemplate();

    void create_ball_grid(vector<Ball>& game_balls); // This is used to create a basic arrangement of balls for each level, this is automatically overridden in the
    // second level using polymorphism to allow for the additional 2 rows of balls.

    void anchor_balls_to_mass(); // This method is used to ensure that all the balls directly connected to the mass, or connected to balls connected to the mass
    // remain in place, and subsequently ensures balls left suspended in the game level fall and are destroyed.

    void handle_fire_cannon_event(float angle); // This is used to define the cannon firing behaviour, including the playing of sounds, the application of velocity
    // to the fired ball, and the reloading of the cannon using a queue system with another ball to replace the last one fired.

public:
    string run_menu(sf::RenderWindow & window, PlayerInput & player_input, string current_menu); // This is used to run the level and is called once per frame. Here the game
    // objects are computed, then rendered and then the menu considers which menu (if necessary) the game should transition to next. Any textures that need loading in will be
    // handled automatically. This is the most complex method in the game, and handles the orchestration of the game level its self.

    void unload(); // used to unload any textures that exist in RAM that are no longer needed after the game transitions onto a different level. The unloading of a menu is handled
    // mainly in 'main.cpp' as here is where menus transition between each other, and as such different menus can be unloaded when its not possible to directly access them from
    // the current menu, ensuring quick transitions and efficient memory usage.
};

class LevelOne: public LevelTemplate {
    // This is the first level of the game, accessed directly from the main menu. This inherits a lot of the common level behaviour
    // from the LevelTemplate class.
public:
    void init(); // This is used instead of the default constructor and works similarly. When called it initializes each of the level objects inhrited from the
    // LevelTemplate class.

    void reset_level(); // This is used to reset the game objects to their initial values, allowing the game to be played multiple times.
};

class LevelTwo: public LevelTemplate {
    // This is the second level of the game, accessed if the user completes the first level (defined above) and also inherits a lot of its common
    // behaviour from the LevelTemplate class.
public:
    void init();

    void reset_level();
};

class GameEndMenu {
    // This menu is accessed when the user wins or looses the game. From here the user can return to the main menu. This menu then serves an important
    // role as it makes the game finite, and also clearly tells the user if they won or lost. It should also be noted that this menu has no game instructions
    // of any kind, as the player will have seen them on at minimum 2 previous menus, also, if the player wins the game, then there would be no need to remind
    // them of the way the game works.
private:
    Text title_text = Text(Constants::FONT_REGULAR);
    Text game_win_state = Text(Constants::FONT_PLAY);
    Text game_score_result = Text(Constants::FONT_PLAY);

    MediaControls media_controls;

    Button play_again_button;
    Button quit_button;

    Dragon left_dragon;
    Dragon right_dragon;

public:
    GameEndMenu();

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void unload();
};

class PauseMenu {
    // This menu is accessed from the levels by either pressing the 'esc' key, or by clicking away from the window. Whilst in this window the progress
    // of the level is paused and cannot be seen, allowing a break from the gameplay. Pressing the 'esc' key, or using the menu's button arrangement
    // can be used to return you to your level, return you to the main menu (resetting your progress) or allowing you to leave the game.
private:
    Text title_text = Text(Constants::FONT_REGULAR);
    MainMenuInstructions instructions;
    Button resume_button;
    Button quit_button;
    Button main_menu_button;
    MediaControls media_controls;

public:
    PauseMenu();

    string run_menu(
        sf::RenderWindow& window,
        string* menu_navigation,
        PlayerInput& player_input); // This menu is slightly different from the others, as this menu in particular will often exit back to the previous
        // menu, so the menu navigation history is also passed in, so the previous menu can be retrieved.

    void unload();
};