#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"

class Mass {
private:
    sf::Texture mass_texture;
    sf::Sprite mass;
    int x_position = 0;
    int y_position = 0;
    float mass_sprite_x_size = 0;
    float mass_sprite_y_size = 0; // floats because used in scaling
    float vertical_offset = 0;
    bool loaded = false;
    string mass_texture_file_path;
    bool first_load = true;
    bool is_falling = true;

public:
    Mass();

    int get_game_ceiling();

    int get_x_position();

    int get_y_position();

    int get_width();

    int get_height();

    void set_vertical_offset(int offset);

    void compute();

    void render(sf::RenderWindow& window);

    void reset();

    void load();

    void unload();

    void set_is_falling(bool is_falling);

    void set_y_position(int y_position);
};

class Ball {
private:
    float x, y;
    int radius;
    sf::Color color;
    float shape_x_velocity = 0;
    float shape_y_velocity = 0;
    sf::CircleShape shape;
    bool popped = false;
    bool group_flag = false;
    bool anchored_flag = false;
    bool ball_to_fall = false;
    bool pop_sound_to_play = false;

public:
    Ball(
        float x,
        float y);

    void set_velocity(float x_velocity, float y_velocity);

    void set_position(float new_x, float new_y);

    void compute(Mass& mass);

    bool check_collision_with_flag_ball(Ball& ball);

    bool check_collision_with_anchor_ball(Ball& ball);

    void collision(Ball& ball, vector<Ball>& game_balls);

    void render(sf::RenderWindow& window);

    bool pop_sound_needs_playing();

    bool get_popped();
    float get_x_velocity();
    float get_y_velocity();
    float get_y_position();
    bool get_anchored();
    bool get_is_ball_to_fall();
    bool get_group_flag();

    void set_anchored(bool is_anchored);
    void set_ball_to_fall(bool is_to_fall);
    void set_group_flag(bool is_grouped);
};

class Cannon {
private:
    bool first_load = true;
    sf::Texture cannon_texture;
    sf::Sprite cannon;
    float rotation = 0;
    int unrotated_cannon_height = 0;
    int cannon_x_position = 0;
    int cannon_y_position = 0;
    bool loaded = false;
    string cannon_texture_file_path;

public:
    Cannon();

    void set_position(int new_x, int new_y, vector<Ball>& game_balls);

    void load_cannon_with_ball(vector<Ball>& game_balls);

    void compute(sf::RenderWindow& window, PlayerInput& player_input, vector<Ball>& game_balls);

    float get_rotation();

    void render(sf::RenderWindow& window);

    void unload();

    void load();

    int get_x_position();

    int get_y_position();

    int get_height();
};