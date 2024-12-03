#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"
#include "GameObjects.h"
#include "GameGraphics.h"

class MainMenu {
    string game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    Text title_text = Text(Constants::FONT_REGULAR);
    Button play_button;
    Button quit_button;
    MainMenuInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;

public:
    MainMenu();

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void unload();
};

class LevelOne {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object;
    Mass mass_object;
    bool game_lost = false;
    LevelInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;

    sf::Music pop_sounds[15];
    sf::Music cannon_fire_sound;
    sf::Music anvil_fail_sound;

public:
    LevelOne();

    void init();

    void create_ball_grid(vector<Ball>& game_balls);

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void reset_level();

    void unload();
};

class LevelTwo {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object;
    Mass mass_object;
    bool game_lost = false;
    LevelInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;

    sf::Music pop_sounds[15];
    sf::Music cannon_fire_sound;
    sf::Music anvil_fail_sound;

public:
    LevelTwo();

    void init();

    void create_ball_grid(vector<Ball>& game_balls);

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void reset_level();

    void unload();
};

class GameEndMenu {
    Text title_text = Text(Constants::FONT_REGULAR);
    Text game_win_state = Text(Constants::FONT_PLAY);
    Text game_score_result = Text(Constants::FONT_PLAY);

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
    Text title_text = Text(Constants::FONT_REGULAR);
    MainMenuInstructions instructions;
    Button resume_button;
    Button quit_button;
    Button main_menu_button;

public:
    PauseMenu();

    string run_menu(
        sf::RenderWindow& window,
        string* menu_navigation,
        PlayerInput& player_input);

    void unload();
};