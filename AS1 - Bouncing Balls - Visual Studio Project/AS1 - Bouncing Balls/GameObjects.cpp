#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4267 )

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "GameObjects.h"
#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"
#include "UtilityFunctions.h"

using namespace std;

Mass::Mass() {
    string mass_texture_path_components[4] = { "resources", "images", "anvil.png" };
    mass_texture_file_path = path_builder(mass_texture_path_components);
}

int Mass::get_x_position() {
    return x_position;
}

int Mass::get_y_position() {
    return y_position;
}

int Mass::get_game_ceiling() {
    return y_position + mass.getGlobalBounds().height;
}

int Mass::get_width() {
    return mass.getGlobalBounds().width;
}

void Mass::set_vertical_offset(int offset) {
    vertical_offset = offset;
}

void Mass::compute() {
    if (loaded == false) {
        load();
    }
    float x_scale = Registry::window_size[1] / mass_sprite_x_size;
    mass.setScale(x_scale, x_scale);

    y_position = -mass.getGlobalBounds().height + vertical_offset;

    x_position = (Registry::window_size[0] - mass.getGlobalBounds().width) / 2;
    vertical_offset += Constants::MASS_FALL_SPEED;//0.1; // this translates into space covered (down) onscreen.
}

void Mass::render(sf::RenderWindow& window) {
    if (loaded == false) {
        load();
    }
    mass.setPosition(x_position, y_position);
    window.draw(mass);
}

void Mass::reset() {
    vertical_offset = 0;
}

void Mass::unload() {
    loaded = false;
    sf::Texture new_mass_texture;
    mass.setTexture(new_mass_texture);
    mass_texture = new_mass_texture;
}

void Mass::load() {
    mass_texture.loadFromFile(mass_texture_file_path);
    mass_texture.setSmooth(true);
    mass.setTexture(mass_texture);

    if (first_load) {
        mass_sprite_x_size = mass.getGlobalBounds().width;
        mass_sprite_y_size = mass.getGlobalBounds().height;
        y_position = -mass.getGlobalBounds().height; // start above the window!!!
        x_position = (Registry::window_size[0] - mass_sprite_x_size) / 2;
        first_load = false;
    }
    loaded = true;
}

Ball::Ball(
    float x,
    float y) : x(x), y(y) {

    color = pick_ball_color();
    shape.setFillColor(color);
    radius = Registry::ball_radius;
    shape.setRadius(Registry::ball_radius);
    shape.setOrigin(radius, radius); // centre of circle is its position
    shape.setPointCount(calculate_point_count(radius));
    shape.setPosition(x, y);
}

void Ball::set_velocity(float x_velocity, float y_velocity) {
    shape_x_velocity = x_velocity;
    shape_y_velocity = y_velocity;
}

void Ball::set_position(float new_x, float new_y) {
    x = new_x;
    y = new_y;
}

void Ball::compute(Mass& mass) {
    // check for a collision with the 'walls' of the game
    int game_y_minimum = mass.get_game_ceiling();
    int game_surface_max_x = mass.get_x_position() + mass.get_width();
    int game_surface_min_x = mass.get_x_position();

    radius = Registry::ball_radius;
    shape.setRadius(Registry::ball_radius);

    if ((x - radius <= game_surface_min_x && shape_x_velocity < 0) || (x + radius >= game_surface_max_x && shape_x_velocity > 0)) {
        shape_x_velocity *= -1;
    }
    if ((y - radius <= game_y_minimum && shape_y_velocity < 0)) {
        shape_y_velocity = 0;
        shape_x_velocity = 0;
    }

    // Keep the ball inside the place it needs to be
    if (x - radius < game_surface_min_x) {
        x = radius + game_surface_min_x;
    }
    if (x + radius > game_surface_max_x) {
        x = game_surface_max_x - radius;
    }
    if (y - radius < game_y_minimum) {
        y = radius + game_y_minimum;
    }
    if (y - radius > Registry::window_size[1]) {
        popped = true;
        y = -100;
        shape_x_velocity = 0;
        shape_y_velocity = 0;
    }

    x += shape_x_velocity;
    y += shape_y_velocity;

    if (ball_to_fall) {
        y += 5;
    }

    if (shape_x_velocity == 0 && shape_y_velocity == 0) {
        y += Constants::MASS_FALL_SPEED;
    }
}

bool Ball::check_collision_with_flag_ball(Ball& ball) {
    if (ball.color == color && ball.group_flag && group_flag == false) {
        // Calculate the distance between the two balls
        float distance = pythagorean_distance(x, y, ball.x, ball.y)-3;
        if (distance <= radius + ball.radius) {
            return true;
        }
    }
    return false;
}

bool Ball::check_collision_with_anchor_ball(Ball& ball) {
    if (ball.anchored_flag && anchored_flag == false) {
        // Calculate the distance between the two balls
        float distance = pythagorean_distance(x, y, ball.x, ball.y) - 3;
        if (distance <= radius + ball.radius) {
            return true;
        }
    }
    return false;
}

void Ball::collision(Ball& ball, vector<Ball>& game_balls) {
    if (ball.popped == false && ball.ball_to_fall == false && ball_to_fall == false) {
        // Calculate the distance between the two balls
        float distance = pythagorean_distance(x, y, ball.x, ball.y);
        if (distance < radius + ball.radius) {
            // Calculate the angle for the collision by assuming a tangent 
            // to each ball, then basic trig
            float angle = atan2(y - ball.y, x - ball.x);
            float sin_angle = sin(angle);
            float cos_angle = cos(angle);

            // Make sure that when the balls collide they are split apart so 
            // they don't stick together, or appear to be inside each other
            float overlap = 0.5 * ((radius + ball.radius) - distance);
            if (shape_x_velocity == 0 && shape_y_velocity == 0 && ball.shape_x_velocity == 0 && ball.shape_y_velocity == 0) {
                x += cos_angle * overlap;
                y += sin_angle * overlap;
                ball.x -= cos_angle * overlap;
                ball.y -= sin_angle * overlap;
            }
            else {
                ball.x -= cos_angle * overlap * 2;
                ball.y -= sin_angle * overlap * 2;
            }
            
            if (ball.shape_x_velocity != 0 || ball.shape_y_velocity != 0) {
                // occurs only when the cannon collides with another ball.
                ball.group_flag = true;
                spread_group_flags(game_balls);
                spread_group_flags(game_balls);
                spread_group_flags(game_balls);
                if (count_group_flags(game_balls) >= 3) {
                    bool changed = true;
                    while (changed) {
                         changed = spread_group_flags(game_balls);
                    }
                    for (int i = 0; i < game_balls.size() - 2; i++) {
                        if (game_balls[i].group_flag) {
                            game_balls[i].popped = true;
                            Registry::score += 50;
                            pop_sound_to_play = true;
                            game_balls[i].pop_sound_to_play = true;
                        }
                    }
                }
                else {
                    for (int i = 0; i < game_balls.size() - 2; i++) {
                        game_balls[i].group_flag = false;
                    }
                }
            }

            // Remove their velocity so the balls stop moving.
            shape_x_velocity = 0;
            shape_y_velocity = 0;
            ball.shape_x_velocity = 0;
            ball.shape_y_velocity = 0;
        }
    }
}

void Ball::render(sf::RenderWindow& window) {
    shape.setPosition(x, y);
    if (popped == false) {
        window.draw(shape);
    }
}

bool Ball::pop_sound_needs_playing() {
    if (pop_sound_to_play) {
        pop_sound_to_play = false;
        return true;
    }
    return false;
}

bool Ball::get_popped() {
    return popped;
}

float Ball::get_x_velocity() {
    return shape_x_velocity;
}

float Ball::get_y_velocity() {
    return shape_y_velocity;
}

float Ball::get_y_position() {
    return y;
}

bool Ball::get_anchored() {
    return anchored_flag;
}

bool Ball::get_is_ball_to_fall() {
    return ball_to_fall;
}

void Ball::set_ball_to_fall(bool is_to_fall) {
    ball_to_fall = is_to_fall;
}

void Ball::set_anchored(bool is_anchored) {
    anchored_flag = is_anchored;
}

bool Ball::get_group_flag() {
    return group_flag;
}

void Ball::set_group_flag(bool is_grouped) {
    group_flag = is_grouped;
}

Cannon::Cannon() {
    string cannon_texture_path_components[4] = { "resources", "images", "cannon.png" };
    cannon_texture_file_path = path_builder(cannon_texture_path_components);
}

void Cannon::set_position(int new_x, int new_y, vector<Ball>& game_balls) {
    cannon_x_position = new_x;
    cannon_y_position = new_y;

    cannon.setPosition(new_x, new_y);

    if (game_balls.size() >= 2) {
        for (int i = game_balls.size() - 2; i < game_balls.size(); ++i) {
            game_balls[i].set_position(new_x, new_y);
        }
    }
}

void Cannon::load_cannon_with_ball(vector<Ball>& game_balls) {
    Ball game_ball = Ball(cannon.getPosition().x, cannon.getPosition().y);
    game_balls.emplace_back(game_ball);
}

void Cannon::compute(sf::RenderWindow& window, PlayerInput& player_input, vector<Ball>& game_balls) {
    // define local variables
    int cannon_width = cannon.getGlobalBounds().width;
    int cannon_height = cannon.getGlobalBounds().height;

    // mouse targeting!
    float opposite = player_input.get_mouse_position()[0] - cannon.getPosition().x;
    float adjacent = abs(player_input.get_mouse_position()[1] - cannon.getPosition().y);
    float new_rotation = 90 + atan(opposite / adjacent) * Constants::RADIANS_TO_DEGREES_CONVERSION_CONSTANT;

    // update cannon translation and rotation
    if (new_rotation < 170 && new_rotation > 10) {
        rotation = new_rotation;
        cannon.setRotation(rotation);
    }

    set_position(Registry::window_size[0] / 2.0, Registry::window_size[1] - unrotated_cannon_height / 2, game_balls);
}

float Cannon::get_rotation() {
    return rotation;
}

void Cannon::render(sf::RenderWindow& window) {
    if (loaded == false) {
        load();
    }
    window.draw(cannon);
}

void Cannon::load() {
    cannon_texture.loadFromFile(cannon_texture_file_path);
    cannon.setTexture(cannon_texture);

    if (first_load) {
        cannon.setPosition(-500, -500); // hide ofscreen until correctly positioned
        cannon.setScale(0.168, 0.168);

        float window_scale = Registry::window_size[1] / 720.0;
        cannon.setScale(0.168 * window_scale, 0.168 * window_scale);
        cannon.setOrigin(366, 366);
        cannon.setRotation(0);
        unrotated_cannon_height = cannon.getGlobalBounds().height;
        first_load = false;
    }

    loaded = true;
}

void Cannon::unload() {
    loaded = false;
    sf::Texture new_cannon_texture;
    cannon.setTexture(new_cannon_texture);
    cannon_texture = new_cannon_texture;
}

int Cannon::get_x_position() {
    return cannon_x_position;
}

int Cannon::get_y_position() {
    return cannon_y_position;
}

int Cannon::get_height() {
    return cannon.getGlobalBounds().height;
}