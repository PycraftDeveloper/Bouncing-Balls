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

};

class LevelOne {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object = Cannon(game_balls);
    Mass mass_object;
    bool level_lost = false;
    LevelInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;

public:
    LevelOne(sf::RenderWindow& window);

    void create_ball_grid(vector<Ball>& game_balls);

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void reset_level(sf::RenderWindow& window);
};

class LevelTwo {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object = Cannon(game_balls);
    Mass mass_object;
    bool level_lost = false;
    LevelInstructions instructions;
    Dragon left_dragon;
    Dragon right_dragon;

public:
    LevelTwo(sf::RenderWindow& window);

    void create_ball_grid(vector<Ball>& game_balls);

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void reset_level(sf::RenderWindow& window);
};

class GameEndMenu {
    Text title_text = Text(Constants::FONT_REGULAR);
    Text game_win_state = Text(Constants::FONT_PLAY);

    Button play_again_button;
    Button quit_button;

    Dragon left_dragon;
    Dragon right_dragon;
public:
    GameEndMenu();

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input, bool game_won);
};

class PauseMenu {
    Text title_text = Text(Constants::FONT_REGULAR);
    MainMenuInstructions instructions;
    Button resume_button;
    Button quit_button;

public:
    PauseMenu();

    string run_menu(
        sf::RenderWindow& window,
        string* menu_navigation,
        PlayerInput& player_input);
};