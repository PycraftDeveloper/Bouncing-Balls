#pragma warning( disable : 4244 )

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "GameMenus.h"
#include "UtilityObjects.h"
#include "GameObjects.h"
#include "Constants.h"
#include "Registry.h"
#include "UtilityFunctions.h"

using namespace std;

MainMenu::MainMenu() {
}

string MainMenu::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
    // this runs once every frame
    // top to bottom rendering here where layering doesn't matter
    // render game title
    instructions.compute();
    instructions.render(window);

    left_dragon.compute(Constants::LEFT);
    left_dragon.render(window);

    right_dragon.compute(Constants::RIGHT);
    right_dragon.render(window);

    title_text.set_position(window, -1, 0);
    title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);

    // render play button
    bool play_button_result;
    play_button_result = play_button.compute(player_input);
    play_button.render(window, -1, -1, "Play!", 24, 30);

    // render quit button
    bool quit_button_result;
    quit_button_result = quit_button.compute(player_input);
    quit_button.render(window, -1, -101, "quit", 24, 20);

    // identify what menu to transition to next
    if (quit_button_result) {
        return Constants::QUIT;
    }
    else if (play_button_result) {
        return Constants::LEVEL_ONE;
    }
    // by default return to this menu on next run
    return Constants::MAIN_MENU;
}

LevelOne::LevelOne(sf::RenderWindow& window) {
    mass_object.compute(window);
    create_ball_grid(game_balls);

    cannon_object.load_cannon_with_ball(game_balls);
    cannon_object.load_cannon_with_ball(game_balls);
}

void LevelOne::create_ball_grid(vector<Ball>& game_balls) {
    int columns = 14;
    int x_pos = Registry::ball_radius;
    int y_pos = Registry::ball_radius + mass_object.get_game_ceiling();
    for (int row = 0; row < 6; row++) {
        if (row % 2 == 0) {
            columns = 14;
            x_pos = Registry::ball_radius;
        }
        else {
            columns = 13;
            x_pos = Registry::ball_radius * 2;
        }
        for (int column = 0; column < columns; column++) {
            Ball game_ball = Ball(x_pos + mass_object.x_position, y_pos);
            game_balls.emplace_back(game_ball);
            x_pos += Registry::ball_radius * 2;
        }
        y_pos += Registry::ball_tesselation_coefficient;
    }
}

string LevelOne::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
    // this runs once every frame
    instructions.compute();
    instructions.render(window);

    left_dragon.compute(Constants::LEFT);
    left_dragon.render(window);

    right_dragon.compute(Constants::RIGHT);
    right_dragon.render(window);

    float angle = 0;
    bool ball_in_motion = false;

    cannon_object.render(window);
    cannon_object.compute(window, player_input, game_balls);

    vector<int> garbage_ball_elements = {};

    int index = 0;
    for (auto& game_ball : game_balls) {
        if (index == game_balls.size() - 2) {
            angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

            int x_pos = -sin(angle) * game_ball.radius / 2;
            int y_pos = -cos(angle) * game_ball.radius / 2;
            game_ball.set_position(cannon_object.cannon.getPosition().x + x_pos, cannon_object.cannon.getPosition().y - y_pos);

        }

        game_ball.compute(mass_object);
        game_ball.render(window);
        if (game_ball.popped) {
            garbage_ball_elements.emplace_back(index);
        }
        else if (game_ball.shape_x_velocity == 0 && game_ball.shape_y_velocity == 0 && index < game_balls.size() - 2) {
            for (auto& collision_game_ball : game_balls) {
                game_ball.collision(collision_game_ball, game_balls);
            }
        }
        index++;
    }

    game_balls[game_balls.size() - 1].render(window);

    sort(garbage_ball_elements.rbegin(), garbage_ball_elements.rend());

    for (int& garbage_ball : garbage_ball_elements) {
        game_balls.erase(game_balls.begin() + garbage_ball);
    }

    /// anchored balls
    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].y - Registry::ball_radius <= mass_object.get_game_ceiling()+10) {
            game_balls[i].anchored_flag = true;
        }
    }

    bool changed = true;
    while (changed) {
        changed = spread_anchor_flags(game_balls);
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].anchored_flag == false && game_balls[i].shape_x_velocity == 0 && game_balls[i].shape_y_velocity == 0) {
            game_balls[i].ball_to_fall = true;
        }
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        game_balls[i].anchored_flag = false;
    }
    ///

    index = 0;
    for (auto& game_ball : game_balls) {
        if (game_ball.shape_x_velocity != 0 || game_ball.shape_y_velocity != 0) {
            ball_in_motion = true;
        }
        if (game_ball.shape_x_velocity == 0 && game_ball.shape_y_velocity == 0 && index < game_balls.size() - 2) {
            if (game_ball.shape.getPosition().y + game_ball.radius >= cannon_object.cannon_y_position - cannon_object.cannon.getGlobalBounds().height / 2) {
                if (game_ball.ball_to_fall == false) {
                    game_lost = true;
                }
            }
        }
        index++;
    }

    if (mass_object.y_position >= cannon_object.cannon_y_position - cannon_object.cannon.getGlobalBounds().height / 2) {
        game_lost = true;
    }

    mass_object.render(window);
    mass_object.compute(window);

    if (player_input.get_player_button_input() && ball_in_motion == false) {
        //play_cannon_fire_sound();
        swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);

        // get the angle the cannon is pointing at and fire the ball in that same direction
        angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

        float x_velocity = sin(angle) * Constants::BALL_SPEED;
        float y_velocity = -cos(angle) * Constants::BALL_SPEED;

        game_balls[game_balls.size() - 2].set_velocity(x_velocity, y_velocity);
        //game_balls[game_balls.size() - 2].set_position(0, 0);

        cannon_object.load_cannon_with_ball(game_balls);
        swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);
    }

    if (game_lost) {
        Registry::game_end_state = Constants::LOST;
        return Constants::END_MENU;
    }
    else if (game_balls.size() == 2) {
            return Constants::LEVEL_TWO;
    }

    return Constants::LEVEL_ONE;
}

void LevelOne::reset_level(sf::RenderWindow& window) {
    game_lost = false;
    mass_object.reset();
    game_balls.clear();
    mass_object.compute(window);
    create_ball_grid(game_balls);

    cannon_object.load_cannon_with_ball(game_balls);
    cannon_object.load_cannon_with_ball(game_balls);
}

LevelTwo::LevelTwo(sf::RenderWindow& window) {
    int ball_diameter = 2 * Registry::ball_radius;
    mass_object.set_vertical_offset(-2 * ball_diameter);
    mass_object.compute(window);

    create_ball_grid(game_balls);

    cannon_object.load_cannon_with_ball(game_balls);
    cannon_object.load_cannon_with_ball(game_balls);
}

void LevelTwo::create_ball_grid(vector<Ball>& game_balls) {
    int columns = 14;
    int x_pos = Registry::ball_radius;
    int y_pos = Registry::ball_radius + mass_object.get_game_ceiling();
    for (int row = 0; row < 8; row++) {
        if (row % 2 == 0) {
            columns = 14;
            x_pos = Registry::ball_radius;
        }
        else {
            columns = 13;
            x_pos = Registry::ball_radius * 2;
        }
        for (int column = 0; column < columns; column++) {
            Ball game_ball = Ball(x_pos + mass_object.x_position, y_pos);
            game_balls.emplace_back(game_ball);
            x_pos += Registry::ball_radius * 2;
        }
        y_pos += Registry::ball_tesselation_coefficient;
    }
}

string LevelTwo::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
    // this runs once every frame
    instructions.compute();
    instructions.render(window);

    left_dragon.compute(Constants::LEFT);
    left_dragon.render(window);

    right_dragon.compute(Constants::RIGHT);
    right_dragon.render(window);

    float angle = 0;
    bool ball_in_motion = false;

    cannon_object.render(window);
    cannon_object.compute(window, player_input, game_balls);

    vector<int> garbage_ball_elements = {};

    int index = 0;
    for (auto& game_ball : game_balls) {
        if (index == game_balls.size() - 2) {
            angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

            int x_pos = -sin(angle) * game_ball.radius / 2;
            int y_pos = -cos(angle) * game_ball.radius / 2;
            game_ball.set_position(cannon_object.cannon.getPosition().x + x_pos, cannon_object.cannon.getPosition().y - y_pos);
        }

        game_ball.compute(mass_object);
        game_ball.render(window);
        if (game_ball.popped) {
            garbage_ball_elements.emplace_back(index);
        }
        else if (game_ball.shape_x_velocity == 0 && game_ball.shape_y_velocity == 0 && index < game_balls.size() - 2) {
            for (auto& collision_game_ball : game_balls) {
                game_ball.collision(collision_game_ball, game_balls);
            }
        }
        index++;
    }

    game_balls[game_balls.size() - 1].render(window);

    sort(garbage_ball_elements.rbegin(), garbage_ball_elements.rend());

    for (int& garbage_ball : garbage_ball_elements) {
        game_balls.erase(game_balls.begin() + garbage_ball);
    }

    /// anchored balls
    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].y - Registry::ball_radius <= mass_object.get_game_ceiling() + 10) {
            game_balls[i].anchored_flag = true;
        }
    }

    bool changed = true;
    while (changed) {
        changed = spread_anchor_flags(game_balls);
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].anchored_flag == false && game_balls[i].shape_x_velocity == 0 && game_balls[i].shape_y_velocity == 0) {
            game_balls[i].ball_to_fall = true;
        }
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        game_balls[i].anchored_flag = false;
    }
    ///

    index = 0;
    for (auto& game_ball : game_balls) {
        if (game_ball.shape_x_velocity != 0 || game_ball.shape_y_velocity != 0) {
            ball_in_motion = true;
        }
        if (game_ball.shape_x_velocity == 0 && game_ball.shape_y_velocity == 0 && index < game_balls.size() - 2) {
            if (game_ball.shape.getPosition().y + game_ball.radius >= cannon_object.cannon_y_position - cannon_object.cannon.getGlobalBounds().height / 2) {
                if (game_ball.ball_to_fall == false) {
                    game_lost = true;
                }
            }
        }
        index++;
    }

    if (mass_object.y_position >= cannon_object.cannon_y_position - cannon_object.cannon.getGlobalBounds().height / 2) {
        game_lost = true;
    }

    mass_object.render(window);
    mass_object.compute(window);

    if (player_input.get_player_button_input() && ball_in_motion == false) {
        //play_cannon_fire_sound();
        swap(game_balls[game_balls.size() - 2], game_balls[game_balls.size() - 1]);

        // get the angle the cannon is pointing at and fire the ball in that same direction
        angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

        float x_velocity = sin(angle) * Constants::BALL_SPEED;
        float y_velocity = -cos(angle) * Constants::BALL_SPEED;

        game_balls[game_balls.size() - 2].set_velocity(x_velocity, y_velocity);
        //game_balls[game_balls.size() - 2].set_position(0, 0);

        cannon_object.load_cannon_with_ball(game_balls);
        swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);
    }

    if (game_lost) {
        Registry::game_end_state = Constants::LOST;
        return Constants::END_MENU;
    }

    if (game_balls.size() == 2) {
        Registry::game_end_state = Constants::WIN;
        return Constants::END_MENU;
    }

    return Constants::LEVEL_TWO;
}

void LevelTwo::reset_level(sf::RenderWindow& window) {
    game_lost = false;
    mass_object.reset();
    game_balls.clear();
    mass_object.compute(window);
    create_ball_grid(game_balls);

    cannon_object.load_cannon_with_ball(game_balls);
    cannon_object.load_cannon_with_ball(game_balls);
}

GameEndMenu::GameEndMenu() {
}

string GameEndMenu::run_menu(sf::RenderWindow& window, PlayerInput& player_input, bool game_won) {
    // this runs once every frame
    // render game title
    left_dragon.compute(Constants::LEFT);
    left_dragon.render(window);

    right_dragon.compute(Constants::RIGHT);
    right_dragon.render(window);

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
    play_again_button.render(window, -1, -1, "Play Again!", 24, 30);

    // render quit button
    bool quit_button_result;
    quit_button_result = quit_button.compute(player_input);
    quit_button.render(window, -1, -101, "quit", 24, 20);

    // identify what menu to transition to next
    if (quit_button_result) {
        return Constants::QUIT;
    }
    else if (play_again_button_result) {
        return Constants::MAIN_MENU;
    }

    // by default return to this menu on next run
    return Constants::END_MENU;
}

PauseMenu::PauseMenu() {
}

string PauseMenu::run_menu(
    sf::RenderWindow& window,
    string* menu_navigation,
    PlayerInput& player_input) {

    // this runs once every frame 
    instructions.compute();
    instructions.render(window);

    title_text.set_position(window, -1, 0);
    title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);

    // render "resume game" button
    bool resume_button_result;
    resume_button_result = resume_button.compute(player_input);
    resume_button.render(window, -1, -1, "Resume Game", 24, 30);

    // render quit button
    bool quit_button_result;
    quit_button_result = quit_button.compute(player_input);
    quit_button.render(window, -1, -101, "quit", 24, 20);

    if (quit_button_result) {
        return Constants::QUIT;
    }

    if (resume_button_result) {
        return menu_navigation[1];
        // go to previous level
    }
    return Constants::PAUSE_MENU; // stay on current window
}