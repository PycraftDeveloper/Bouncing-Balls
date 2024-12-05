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
    bool play_button_result, quit_button_result;
    instructions.compute();
    left_dragon.compute(Constants::LEFT);
    right_dragon.compute(Constants::RIGHT);
    play_button_result = play_button.compute(player_input);
    quit_button_result = quit_button.compute(player_input);
    
    title_text.set_position(window, -1, 0);

    instructions.render(window);
    left_dragon.render(window);
    right_dragon.render(window);
    title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);
    play_button.render(window, -1, -1, "Play!", 24, 30);
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

void MainMenu::unload() {
    left_dragon.unload();
    right_dragon.unload();
    instructions.unload();
    play_button.unload();
    quit_button.unload();
    title_text.unload();
}

LevelOne::LevelOne() {
    string cannon_fire_sound_path_components[4] = { "resources", "sounds", "cannon fire.ogg" };
    string anvil_fail_sound_path_components[4] = { "resources", "sounds", "anvil.ogg" };

    for (int i = 0; i < 15; i++) { // 16
        string file = to_string(i + 1) + ".ogg";
        string pop_sound_path_components[4] = { "resources", "sounds", "pops", file };
        string file_path = path_builder(pop_sound_path_components);
        pop_sounds[i].openFromFile(file_path);
    }
    
    cannon_fire_sound.openFromFile(path_builder(cannon_fire_sound_path_components));
    cannon_fire_sound.setVolume(30);
    
    anvil_fail_sound.openFromFile(path_builder(anvil_fail_sound_path_components));
    anvil_fail_sound.setVolume(40);
}

void LevelOne::init() {
    mass_object.compute();

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
            Ball game_ball = Ball(x_pos + mass_object.get_x_position(), y_pos);
            game_balls.emplace_back(game_ball);
            x_pos += Registry::ball_radius * 2;
        }
        y_pos += Registry::ball_tesselation_coefficient;
    }
}

string LevelOne::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
    // this runs once every frame
    float angle = 0;
    bool ball_in_motion = false;
    vector<int> garbage_ball_elements = {};
    int index = 0;
    int random_pop_sound_index, x_pos, y_pos;

    instructions.compute();
    left_dragon.compute(Constants::LEFT);
    right_dragon.compute(Constants::RIGHT);

    if (level_over == false) {
        cannon_object.compute(window, player_input, game_balls);
    }

    instructions.render(window);
    left_dragon.render(window);
    right_dragon.render(window);

    if (level_over == false) {
        cannon_object.render(window);
    }

    if (level_over == false) {
        for (auto& game_ball : game_balls) {
            if (index == game_balls.size() - 2) {
                angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

                x_pos = -sin(angle) * Registry::ball_radius / 2;
                y_pos = -cos(angle) * Registry::ball_radius / 2;
                game_ball.set_position(cannon_object.get_x_position() + x_pos, cannon_object.get_y_position() - y_pos);

            }

            game_ball.compute(mass_object);
            game_ball.render(window);
            if (game_ball.get_popped()) {
                garbage_ball_elements.emplace_back(index);
            }
            else if (game_ball.get_x_velocity() == 0 && game_ball.get_y_velocity() == 0 && index < game_balls.size() - 2) {
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

        anchor_balls_to_mass();

        index = 0;
        for (auto& game_ball : game_balls) {
            if (game_ball.get_x_velocity() != 0 || game_ball.get_y_velocity() != 0) {
                ball_in_motion = true;
            }
            if (game_ball.get_x_velocity() == 0 && game_ball.get_y_velocity() == 0 && index < game_balls.size() - 2) {
                if (game_ball.get_y_position() + Registry::ball_radius >= cannon_object.get_y_position() - cannon_object.get_height() / 2) {
                    if (game_ball.get_is_ball_to_fall() == false) {
                        game_lost = true;
                        level_over = true;
                    }
                }
            }

            if (game_ball.pop_sound_needs_playing()) {
                random_pop_sound_index = rand() % 15;
                pop_sounds[random_pop_sound_index].play();
            }

            index++;
        }

        if (mass_object.get_y_position() >= cannon_object.get_y_position() - cannon_object.get_height() / 2) {
            game_lost = true;
            level_over = true;
        }
    }
    mass_object.render(window);
    mass_object.compute();

    if (level_over == false) {
        if (player_input.get_player_button_input() && ball_in_motion == false) {
            handle_fire_cannon_event(angle);
        }
    }

    if (game_balls.size() == 2) {
        level_over = true;
    }

    if (level_end_time == -1) {
        if (game_lost) {
            level_end_time = Registry::run_time;
            anvil_fail_sound.play();
            mass_object.set_is_falling(false);
            mass_object.set_y_position(Registry::window_size[1] - mass_object.get_height());
            for (int i = 0; i < 15; i++) {
                // play each pop sound once!
                random_pop_sound_index = rand() % 15;
                pop_sounds[random_pop_sound_index].play();
            }
        }
        else if (level_over) {
            mass_object.set_is_falling(false);
            level_end_time = Registry::run_time;
        }
    }

    if (level_over && game_lost == false) {
        level_complete_text.set_position(window, -1, -1);
        level_complete_text.render(window, "Level Complete!", 50, sf::Color(60, 176, 67), true);
    }

    if (Registry::run_time - level_end_time > 3) {
        if (game_lost) {
            Registry::game_end_state = Constants::LOST;
            return Constants::END_MENU;
        }
        else if (level_over) {
            return Constants::LEVEL_TWO;
        }
    }

    return Constants::LEVEL_ONE;
}

void LevelOne::anchor_balls_to_mass() {
    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].get_y_position() - Registry::ball_radius <= mass_object.get_game_ceiling() + 10) {
            game_balls[i].set_anchored(true);
        }
    }

    bool changed = true;
    while (changed) {
        changed = spread_anchor_flags(game_balls);
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].get_anchored() == false && game_balls[i].get_x_velocity() == 0 && game_balls[i].get_y_velocity() == 0 && game_balls[i].get_is_ball_to_fall() == false) {
            game_balls[i].set_ball_to_fall(true);
            Registry::score += 50;
        }
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        game_balls[i].set_anchored(false);
    }
}

void LevelOne::handle_fire_cannon_event(float angle) {
    cannon_fire_sound.play();
    swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);

    float x_velocity = sin(angle) * Constants::BALL_SPEED;
    float y_velocity = -cos(angle) * Constants::BALL_SPEED;

    game_balls[game_balls.size() - 2].set_velocity(x_velocity, y_velocity);
    //game_balls[game_balls.size() - 2].set_position(0, 0);

    cannon_object.load_cannon_with_ball(game_balls);
    swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);
}

void LevelOne::reset_level() {
    game_lost = false;
    level_over = false;
    level_end_time = -1;

    mass_object.reset();

    game_balls.clear();

    mass_object.compute();

    create_ball_grid(game_balls);

    cannon_object.load_cannon_with_ball(game_balls);
    cannon_object.load_cannon_with_ball(game_balls);
}

void LevelOne::unload() {
    cannon_object.unload();
    mass_object.unload();
    left_dragon.unload();
    right_dragon.unload();
    instructions.unload();
}

LevelTwo::LevelTwo() {
    string cannon_fire_sound_path_components[4] = { "resources", "sounds", "cannon fire.ogg" };
    string anvil_fail_sound_path_components[4] = { "resources", "sounds", "anvil.ogg" };

    for (int i = 0; i < 15; i++) { // 16
        string file = to_string(i + 1) + ".ogg";
        string pop_sound_path_components[4] = { "resources", "sounds", "pops", file };
        string file_path = path_builder(pop_sound_path_components);
        pop_sounds[i].openFromFile(file_path);
    }

    cannon_fire_sound.openFromFile(path_builder(cannon_fire_sound_path_components));
    cannon_fire_sound.setVolume(30);

    anvil_fail_sound.openFromFile(path_builder(anvil_fail_sound_path_components));
    anvil_fail_sound.setVolume(40);
}

void LevelTwo::init() {
    int ball_diameter = 2 * Registry::ball_radius;

    mass_object.set_vertical_offset(-2 * ball_diameter);

    mass_object.compute();

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
            Ball game_ball = Ball(x_pos + mass_object.get_x_position(), y_pos);
            game_balls.emplace_back(game_ball);
            x_pos += Registry::ball_radius * 2;
        }
        y_pos += Registry::ball_tesselation_coefficient;
    }
}

string LevelTwo::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
    // this runs once every frame

    float angle = 0;
    bool ball_in_motion = false;
    vector<int> garbage_ball_elements = {};
    int index = 0;
    int random_pop_sound_index, x_pos, y_pos;

    instructions.compute();
    right_dragon.compute(Constants::RIGHT);
    left_dragon.compute(Constants::LEFT);
    if (level_over == false) {
        cannon_object.compute(window, player_input, game_balls);
    }

    left_dragon.render(window);
    instructions.render(window);
    right_dragon.render(window);

    if (level_over == false) {
        cannon_object.render(window);
    }

    if (level_over == false) {
        for (auto& game_ball : game_balls) {
            if (index == game_balls.size() - 2) {
                angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

                x_pos = -sin(angle) * Registry::ball_radius / 2;
                y_pos = -cos(angle) * Registry::ball_radius / 2;
                game_ball.set_position(cannon_object.get_x_position() + x_pos, cannon_object.get_y_position() - y_pos);
            }

            game_ball.compute(mass_object);
            game_ball.render(window);
            if (game_ball.get_popped()) {
                garbage_ball_elements.emplace_back(index);
            }
            else if (game_ball.get_x_velocity() == 0 && game_ball.get_y_velocity() == 0 && index < game_balls.size() - 2) {
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

        anchor_balls_to_mass();

        index = 0;
        for (auto& game_ball : game_balls) {
            if (game_ball.get_x_velocity() != 0 || game_ball.get_y_velocity() != 0) {
                ball_in_motion = true;
            }
            if (game_ball.get_x_velocity() == 0 && game_ball.get_y_velocity() == 0 && index < game_balls.size() - 2) {
                if (game_ball.get_y_position() + Registry::ball_radius >= cannon_object.get_y_position() - cannon_object.get_height() / 2) {
                    if (game_ball.get_is_ball_to_fall() == false) {
                        game_lost = true;
                    }
                }
            }

            if (game_ball.pop_sound_needs_playing()) {
                random_pop_sound_index = rand() % 15;
                pop_sounds[random_pop_sound_index].play();
            }

            index++;
        }

        if (mass_object.get_y_position() >= cannon_object.get_y_position() - cannon_object.get_height() / 2) {
            game_lost = true;
        }
    }
    mass_object.render(window);
    mass_object.compute();

    if (level_over == false) {
        if (player_input.get_player_button_input() && ball_in_motion == false) {
            handle_fire_cannon_event(angle);
        }
    }

    if (game_balls.size() == 2) {
        level_over = true;
    }

    if (level_end_time == -1) {
        if (game_lost) {
            level_end_time = Registry::run_time;
            anvil_fail_sound.play();
            mass_object.set_is_falling(false);
            mass_object.set_y_position(Registry::window_size[1] - mass_object.get_height());
            for (int i = 0; i < 15; i++) {
                // play each pop sound once!
                random_pop_sound_index = rand() % 15;
                pop_sounds[random_pop_sound_index].play();
            }
        }
        else if (level_over) {
            mass_object.set_is_falling(false);
            level_end_time = Registry::run_time;
        }
    }

    if (level_over && game_lost == false) {
        level_complete_text.set_position(window, -1, -1);
        level_complete_text.render(window, "Level Complete!", 50, sf::Color(60, 176, 67), true);
    }

    if (Registry::run_time - level_end_time > 3) {
        if (game_lost) {
            Registry::game_end_state = Constants::LOST;
            return Constants::END_MENU;
        }
        else if (level_over) {
            Registry::game_end_state = Constants::WIN;
            return Constants::END_MENU;
        }
    }

    return Constants::LEVEL_TWO;
}

void LevelTwo::anchor_balls_to_mass() {
    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].get_y_position() - Registry::ball_radius <= mass_object.get_game_ceiling() + 10) {
            game_balls[i].set_anchored(true);
        }
    }

    bool changed = true;
    while (changed) {
        changed = spread_anchor_flags(game_balls);
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].get_anchored() == false && game_balls[i].get_x_velocity() == 0 && game_balls[i].get_y_velocity() == 0 && game_balls[i].get_is_ball_to_fall() == false) {
            game_balls[i].set_ball_to_fall(true);
            Registry::score += 50;
        }
    }

    for (int i = 0; i < game_balls.size() - 2; i++) {
        game_balls[i].set_anchored(false);
    }
}

void LevelTwo::handle_fire_cannon_event(float angle) {
    cannon_fire_sound.play();
    swap(game_balls[game_balls.size() - 2], game_balls[game_balls.size() - 1]);

    float x_velocity = sin(angle) * Constants::BALL_SPEED;
    float y_velocity = -cos(angle) * Constants::BALL_SPEED;

    game_balls[game_balls.size() - 2].set_velocity(x_velocity, y_velocity);
    //game_balls[game_balls.size() - 2].set_position(0, 0);

    cannon_object.load_cannon_with_ball(game_balls);
    swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);
}

void LevelTwo::reset_level() {
    int ball_diameter = 2 * Registry::ball_radius;

    game_lost = false;
    level_over = false;
    level_end_time = -1;

    mass_object.reset();

    game_balls.clear();

    mass_object.set_vertical_offset(-2 * ball_diameter);
    mass_object.compute();

    create_ball_grid(game_balls);

    cannon_object.load_cannon_with_ball(game_balls);
    cannon_object.load_cannon_with_ball(game_balls);
}

void LevelTwo::unload() {
    cannon_object.unload();
    mass_object.unload();
    left_dragon.unload();
    right_dragon.unload();
    instructions.unload();
}

GameEndMenu::GameEndMenu() {
}

string GameEndMenu::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
    // this runs once every frame
    // render game title
    bool play_again_button_result, quit_button_result;

    left_dragon.compute(Constants::LEFT);
    right_dragon.compute(Constants::RIGHT);
    play_again_button_result = play_again_button.compute(player_input);
    quit_button_result = quit_button.compute(player_input);

    title_text.set_position(window, -1, 0);
    game_win_state.set_position(window, -1, 150); // render game win state (win/loose)
    game_score_result.set_position(window, -1, 200);

    string game_win_state_text_content;
    sf::Color game_win_state_text_color;
    if (Registry::game_end_state == Constants::WIN) {
        game_win_state_text_content = "You WIN!!!";
        game_win_state_text_color = sf::Color(60, 176, 67); // darker shade of green
    }
    else {
        game_win_state_text_content = "You Lost!!!";
        game_win_state_text_color = sf::Color::Red;
    }
    
    left_dragon.render(window);
    right_dragon.render(window);
    title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);
    game_win_state.render(window, game_win_state_text_content, 50, game_win_state_text_color, true);
    game_score_result.render(window, "Your Score: " + to_string(Registry::score), 50, sf::Color::Black);
    play_again_button.render(window, -1, -1, "Play Again!", 24, 30);
    quit_button.render(window, -1, -101, "Quit", 24, 20);

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

void GameEndMenu::unload() {
    left_dragon.unload();
    right_dragon.unload();
    title_text.unload();
    game_win_state.unload();
    game_score_result.unload();
    play_again_button.unload();
    quit_button.unload();
}

PauseMenu::PauseMenu() {
}

string PauseMenu::run_menu(
    sf::RenderWindow& window,
    string* menu_navigation,
    PlayerInput& player_input) {
    // this runs once every frame 
    bool resume_button_result, main_menu_button_result, quit_button_result;

    instructions.compute();
    resume_button_result = resume_button.compute(player_input);
    main_menu_button_result = main_menu_button.compute(player_input);
    quit_button_result = quit_button.compute(player_input);

    title_text.set_position(window, -1, 0);

    instructions.render(window);
    title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);
    resume_button.render(window, -1, -1, "Resume Game", 24, 30);
    main_menu_button.render(window, -1, -101, "Main Menu", 24, 20);
    quit_button.render(window, -1, -201, "Quit", 24, 20);

    if (quit_button_result) {
        return Constants::QUIT;
    }

    if (main_menu_button_result) {
        return Constants::MAIN_MENU;
    }

    if (resume_button_result) {
        return menu_navigation[1];
        // go to previous level
    }
    return Constants::PAUSE_MENU; // stay on current window
}

void PauseMenu::unload() {
    instructions.unload();
    title_text.unload();
    resume_button.unload();
    quit_button.unload();
    main_menu_button.unload();
}