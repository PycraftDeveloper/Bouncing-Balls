/*
This program stores each of the components used in the levels. In the game, the majority of the game mechanics are defined in their
corresponding objects, so these are very important for the correct game mechanics. This is the program file for the structure
outlined in the file 'GameObjects.h'.
*/
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
    // The constructor here is simple, as it simply is used to create the operating system path to the file containing the anvil texture.
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

int Mass::get_height() {
    return mass.getGlobalBounds().height;
}

void Mass::set_vertical_offset(int offset) {
    vertical_offset = offset;
}

void Mass::set_is_falling(bool is_falling) {
    Mass::is_falling = is_falling;
}

void Mass::set_y_position(int y_position) {
    Mass::y_position = y_position;
}

void Mass::compute() {
    if (loaded == false) {
        load();
    }
    float x_scale = Registry::window_size[1] / mass_sprite_x_size; // This ensures that the (square) texture is scaled to fit perfectly in the
    // level area. This is also a square area with dimensions: (window_size[1], window_size[1]). This perfect fit ensures that the mass acts as the
    // ceiling for the game and no balls can move past it.
    mass.setScale(x_scale, x_scale);

    if (is_falling) {
        y_position = -mass.getGlobalBounds().height + vertical_offset;
    } // If the ball is meant to be falling, its y_position is set to the initial y position then moved down by the vertical_offset

    x_position = (Registry::window_size[0] - mass.getGlobalBounds().width) / 2; // Set the mass to appear centred in the window, because the level area
    // is also centred in the window.

    if (is_falling) {
        vertical_offset += Constants::MASS_FALL_SPEED; // this translates into space covered (down) on-screen.
    } // The vertical offset value is increased slightly, ensuring the mass continues to move down every frame.
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
    is_falling = true;
    // this resets the mass, so when the level resets, it can be played again, as the mass largely determines the game overall state and if the
    // level should continue or end.
}

void Mass::unload() {
    loaded = false;
    sf::Texture new_mass_texture;
    mass.setTexture(new_mass_texture);
    mass_texture = new_mass_texture;
}

void Mass::load() {
    mass_texture.loadFromFile(mass_texture_file_path);
    mass_texture.setSmooth(true); // This texture is set to be 'smooth' to try and ensure that the texture scales better as it shrinks to fit the window
    // size.
    mass.setTexture(mass_texture);

    if (first_load) {
        mass_sprite_x_size = mass.getGlobalBounds().width;
        mass_sprite_y_size = mass.getGlobalBounds().height;
        y_position = -mass.getGlobalBounds().height; // start above the window!!!
        x_position = (Registry::window_size[0] - mass_sprite_x_size) / 2;
        first_load = false;
        // Only when the texture is first loaded should the mass have its dimensions changed. This is because when the texture is loaded, unloaded and reloaded
        // the sprite dimensions only change when thee dimension of the texture changes, which would only occur on the first load. This means that on subsequent
        // loads, the sprite will continue to shrink ever smaller unless a check is made to ensure the texture has already been loaded.
    }
    loaded = true;
}

Ball::Ball(
    float x,
    float y) : x(x), y(y) {

    color = pick_ball_color(); // each ball in the game must be either: Red, Yellow or Blue. This is very important as it is used to determine the behaviour of this
    // ball with other balls in the scene later on.
    shape.setFillColor(color);
    radius = Registry::ball_radius;
    shape.setRadius(Registry::ball_radius);
    shape.setOrigin(radius, radius); // centre of circle is its position, which makes manipulating or working with the circle position much easier.

    shape.setPointCount(calculate_point_count(radius)); // set the number of points in the circle to be just enough to accurately represent the circle geometry
    // without wasting RAM on storing unnecessary vertex data.
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
    int game_y_minimum = mass.get_game_ceiling(); // The bottom of the mass object forms the top of the game area.
    int game_surface_max_x = mass.get_x_position() + mass.get_width();
    int game_surface_min_x = mass.get_x_position();
    // The min and max values for the x axis can be easily calculated using the size and position of the mass object, instead of
    // needing to go back to its mathematical origin.

    float time_difference; // used to store the difference between the current time and when the ball started falling, for physics calculations.

    radius = Registry::ball_radius;
    shape.setRadius(Registry::ball_radius);

    if ((x - radius <= game_surface_min_x && shape_x_velocity < 0) || (x + radius >= game_surface_max_x && shape_x_velocity > 0)) {
        shape_x_velocity *= -1;
        // If the ball collides with the x border on the window, it's x velocity component should be inverted, indicating that the ball bounced.
    }
    if ((y - radius <= game_y_minimum && shape_y_velocity < 0)) {
        shape_y_velocity = 0;
        shape_x_velocity = 0;
        // Collisions with the top and bottom of the game area are more complex. The top of the window will cause the ball to immediately 'stick' if it
        // collides. As for the bottom of the game area, there is no collisions here as the balls fall off the bottom of the screen as a part of the game mechanics.
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
    // The 3 conditions above (and some of the condition immediately below) are used to ensure that when a collision occurs with the game area, the ball never appears
    // to have 'escaped' this border, moving it back in, before this detail is rendered.
    if (y - radius > Registry::window_size[1]) {
        popped = true;
        y = -100;
        shape_x_velocity = 0;
        shape_y_velocity = 0;
        // Here, conditions are accounted for when the ball exits the bottom of the frame, ensuring it is correctly cleared from memory later.
    }

    x += shape_x_velocity;
    y += shape_y_velocity;
    // The shapes position is changed by its velocity every frame.

    if (ball_to_fall) {
        if (ball_fall_start_time == 0) {
            ball_fall_start_time = Registry::run_time;
        }
        time_difference = Registry::run_time - ball_fall_start_time;
        y += Constants::MASS_FALL_SPEED * time_difference + Constants::ACCELERATION_DUE_TO_GRAVITY * pow(time_difference, 2);
        // s = ut + 0.5at**2
        // u = Constants::MASS_FALL_SPEED
        // t = time_difference
        // a = Constants::ACCELERATION_DUE_TO_GRAVITY

        // should the ball be due to fall from its position in the game, because it is no longer supported, then this ensures that fall is modelled by the
        // same physics that dictates how objects fall in a 'free-fall' situation on the Earth's surface.
    }
    else {

        if (shape_x_velocity == 0 && shape_y_velocity == 0) {
            y += Constants::MASS_FALL_SPEED;
        } // If the ball is not moving, then it should move down every frame the same amount that the mass moves down, to ensure the balls don't get
        // 'squished' out of their initial arrangement, which would otherwise cause unexpected behaviour to occur.
    }
}

bool Ball::check_collision_with_flag_ball(Ball& ball) {
    if (ball.color == color && ball.group_flag && group_flag == false && ball_to_fall == false) {
        // Calculate the distance between the two balls
        float distance = pythagorean_distance(x, y, ball.x, ball.y)-3;
        if (distance <= radius + ball.radius) {
            return true;
        } // If the ball is of the same colour type, not already a part of the group, and not falling from the scene
        // and is also in collisions with this other ball in the scene, then true is returned. Otherwise false is returned
    }
    return false;
}

bool Ball::check_collision_with_anchor_ball(Ball& ball) {
    if (ball.anchored_flag && anchored_flag == false && ball_to_fall == false) {
        // Calculate the distance between the two balls
        float distance = pythagorean_distance(x, y, ball.x, ball.y) - 3;
        if (distance <= radius + ball.radius) {
            return true;
        } // This works similarly to the method above, except this time no differentiation is made on ball colour, and instead of checking
        // for the group flag, the anchor flag is checked instead.
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
                // This ensures that the balls remain the correct distance from each other when they both are stationary within the scene.
            }
            else {
                ball.x -= cos_angle * overlap * 2;
                ball.y -= sin_angle * overlap * 2;
                // This ensures that the ball fired from the cannon does not intersect with any of the balls in the level.
            }
            
            if (ball.shape_x_velocity != 0 || ball.shape_y_velocity != 0) {
                // occurs only when the cannon collides with another ball.
                ball.group_flag = true;
                // The ball fired acts as the starting point for the group.
                spread_group_flags(game_balls);
                spread_group_flags(game_balls);
                spread_group_flags(game_balls);
                // The flag is spread 3 times. In each spread the flag can spread between 0 and an unlimited number of times. This is to
                // ensure that all the balls that form the group are eventually flagged, but these 3 flags initially are used to determine if a group
                // of balls has been formed.
                if (count_group_flags(game_balls) >= 3) {
                    // If there is a group of 3 or more balls.
                    bool changed = true;
                    while (changed) {
                         changed = spread_group_flags(game_balls); // spread the flag to all the balls that meet the game mechanic requirement
                         // and then only stop spreading the flag when there are no more changes to where the flag has been spread to.
                    }
                    for (int i = 0; i < game_balls.size() - 2; i++) {
                        if (game_balls[i].group_flag) {
                            game_balls[i].popped = true;
                            pop_sound_to_play = true;
                            game_balls[i].pop_sound_to_play = true;
                        }
                    } // Then for each ball in the scene, check to see if it has been flagged. If they have been, then they are marked for deletion from the scene,
                    // and an audible cue is made.
                }
                else {
                    for (int i = 0; i < game_balls.size() - 2; i++) {
                        game_balls[i].group_flag = false;
                    } // if the balls don't form a group of 3 or more, then the flag must be reset on all the balls not owned by the cannon
                    // to ensure that the flag isn't incorrectly spread the next time this code gets ran.
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
    } // Whenever the cannon needs its position changed, this position change also needs to be extended to the balls it 'owns'
    // as otherwise the balls would be correctly arranged but incorrectly positioned inside the cannon.
}

void Cannon::load_cannon_with_ball(vector<Ball>& game_balls) {
    Ball game_ball = Ball(cannon.getPosition().x, cannon.getPosition().y);
    game_balls.emplace_back(game_ball);
    // When the cannon needs a new ball, it is instantiated and added to the front of the game_balls array
    // whilst this works well, it must also be considered that the last two balls in the array form a queue, so
    // the last two balls will need to be re-arranged so the oldest ball is next to fire.
    // index: 0, 1, 2, 3, 4
    // 0 - oldest/back
    // 4 - newest/front
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
    } // when the cannon is rotated, it can only point between these two ranges, to ensure that the cannon cannot fire downwards, or into its self
    // as this behaviour is also seen in the original game.

    set_position(Registry::window_size[0] / 2.0, Registry::window_size[1] - unrotated_cannon_height / 2, game_balls); // When the cannon gets rotated, its size is
    // modified, causing it to move, so it needs to be translated back to the right position again here.
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
        cannon.setPosition(-500, -500); // hide of-screen until correctly positioned
        cannon.setScale(0.168, 0.168);

        float window_scale = Registry::window_size[1] / 720.0;
        cannon.setScale(0.168 * window_scale, 0.168 * window_scale);
        cannon.setOrigin(366, 170); // the origin is set to the centre of the original texture size.
        cannon.setRotation(0);
        unrotated_cannon_height = cannon.getGlobalBounds().height;
        first_load = false;
    } // Similar to the mass class loading system, the cannon sprite only needs to have it's size set on the first load, after that its size is not adjusted
    // as the texture is of the same dimensions when reapplied.

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