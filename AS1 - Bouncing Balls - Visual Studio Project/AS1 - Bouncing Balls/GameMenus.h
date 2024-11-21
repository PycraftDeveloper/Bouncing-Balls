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
    Button play_button = Button();
    Button quit_button = Button();
    MainMenuInstructions instructions;

public:
    MainMenu();

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

};

class LevelOne {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object = Cannon(game_balls);
    Mass mass_object = Mass();
    bool level_lost = false;
    LevelInstructions instructions;

public:
    LevelOne(sf::RenderWindow& window);

    void create_ball_grid(vector<Ball>& game_balls);

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void reset_level(sf::RenderWindow& window);
};

class LevelTwo {
    vector<Ball> game_balls; // last 2 ALWAYS belong to the cannon for firing.
    Cannon cannon_object = Cannon(game_balls);
    Mass mass_object = Mass();
    bool level_lost = false;
    LevelInstructions instructions;

public:
    LevelTwo(sf::RenderWindow& window);

    void create_ball_grid(vector<Ball>& game_balls);

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input);

    void reset_level(sf::RenderWindow& window);
};

class GameEndMenu {
    Text title_text = Text(Constants::FONT_REGULAR);
    Text game_win_state = Text(Constants::FONT_PLAY);

    Button play_again_button = Button();
    Button quit_button = Button();
public:
    GameEndMenu();

    string run_menu(sf::RenderWindow& window, PlayerInput& player_input, bool game_won);
};

class PauseMenu {
    Text title_text = Text(Constants::FONT_REGULAR);
    MainMenuInstructions instructions;

public:
    PauseMenu();

    string run_menu(
        sf::RenderWindow& window,
        string* menu_navigation,
        PlayerInput& player_input);
};