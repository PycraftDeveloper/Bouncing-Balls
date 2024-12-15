/*
This program stores each of the components used in the levels. In the game, the majority of the game mechanics are defined in their
corresponding objects, so these are very important for the correct game mechanics. This is the header file and the actual code content
is found in: 'GameObjects.cpp'.
*/
#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"

class Mass {
    // This class is used to create the mass game object that moves steadily down from the top of the level to the bottom.
private:
    sf::Texture mass_texture;
    sf::Sprite mass;
    int x_position = 0;
    int y_position = 0;
    // This represents the position of the mass on the screen. Initially it is at (0, 0), however this is later updated.
    float mass_sprite_x_size = 0;
    float mass_sprite_y_size = 0;
    // This represents the size of the sprites in their x and y dimensions. This is stored as a float as the sprite is scaled
    // to an inexact size, and it was determined that an approximation here caused unexpected behaviour in game.
    float vertical_offset = 0;
    // This represents the vertical displacement of the mass through the level. This has to be a float as the distance moved per
    // frame (to ensure a smooth motion) are very small and not accurately representable as an integer.
    bool loaded = false; // used to determine if the mass texture is currently loaded into RAM or not.
    string mass_texture_file_path; // stores the completed path to identify the mass texture on disk.
    bool first_load = true; // used to determine if the mass is being loaded for the first time. Because this sprite is scaled
    // to fit perfectly into the game level, this is stored within SFML. Then when the mass texture is loaded again, this stored
    // scaling is reapplied. Attempting to rescale the image further will not result in the intended effect, so the texture is only
    // scaled when first loaded into the scene.
    bool is_falling = true; // used to control if the mass should be moving down the game scene. This is important as when the game level
    // is over (in either win/loose conditions) the game hangs on that frame to make the game state clear to the user. At this time, the
    // game level should appear 'paused' to avoid any undefined behaviour.

public:
    Mass();

    int get_game_ceiling(); // used to get the position of the mass, but instead of from the top of the sprite
    // from the bottom, as this distance will be the new top of the game level as it slowly descends.

    int get_x_position();

    int get_y_position();

    int get_width();

    int get_height();

    void set_vertical_offset(int offset); // used to overwrite the default 0 for the vertical offset of the mass. This is used in the second
    // level, where the mass starts above the top of the window so the user isn't immediately shown the whole game scene.

    void compute(); // used to handle positioning changes for the mass throughout the game.

    void render(sf::RenderWindow& window); // used to render the mass sprite to the window.

    void reset(); // sets the mass back to the top of the scene, and this is important as otherwise attempting to reset a level
    // would result in the game immediately showing a fail if the previous game ended in a fail, as the position of the mass needs to change.

    void load(); // loads the mass texture from disk into RAM when needed 

    void unload(); // removed the mass texture from RAM when no longer needed, saving on RAM usage.

    void set_is_falling(bool is_falling); // used to control when to, and when not to allow the mass to fall. This is also reset with the
    // reset function.

    void set_y_position(int y_position); // used to set the y-position of the mass on screen. 
};

class Ball {
    // This class is used to define the balls for the game level. Including those in the level and the ones 'owned' by the cannon.
private:
    float x, y; // this stores the x and y positions for the ball in the game window.
    int radius; // this stores the radius of the ball
    sf::Color color; // this stores the fill colour for the ball (there is no defined outline colour).
    float shape_x_velocity = 0;
    float shape_y_velocity = 0;
    // The x and y velocities are used for determining how much the ball should move per frame, and also allows for more realistic
    // collisions with the walls confining the game level. The velocity is set by calculating the angle when the ball is fired from the cannon.
    // a ball with velocity (0, 0) in both x and y axis' can be considered as a ball in the level, rather than a ball in motion, adjusting its behaviour slightly.
    sf::CircleShape shape;
    bool popped = false; // used to determine if this ball has been popped or now. Having the ball flag set to popped immediately prevents other
    // balls from interacting with it, and the ball will be removed from RAM by the next frame as its no longer needed.
    bool group_flag = false; // used in part to determine if this ball is part of a larger group of balls. In this game 3 or more balls need to be
    // grouped together, before this flag has any effect.
    bool anchored_flag = false; // this is used to determine if this ball is connected to the top of the level (the mass pushing down) directly
    // or through other connected balls.
    bool ball_to_fall = false; // this is used to determine if the ball is to ball out of the level under acceleration due to gravity. This also
    // immediately prevents other balls from interacting with this ball, and the ball will be removed from the scene, freeing up RAM when the ball
    // is off the bottom of the screen.
    bool pop_sound_to_play = false; // determines if the ball needs to have a pop sound played or not. Once played, this is immediately reset back.
    float ball_fall_start_time = 0; // This is used to determine when the ball started to fall. This is initially set to a default value of '0' which
    // the game considers as the ball not having started falling. This is used in calculating the balls new position in the frame using a simple free fall
    // equation.
    int fall_distance = 0; // similar to the vertical offset in the mass class, this is an offset that determines how far to displace the ball as it falls
    // during free fall when no longer supported.

public:
    Ball(
        float x,
        float y);

    void set_velocity(float x_velocity, float y_velocity); // sets the velocity of the ball, this is only ever called when the ball is fired from the
    // cannon.

    void set_position(float new_x, float new_y); // sets the position of the ball. This is either used in the initial setting up of the level
    // when arranging the balls in the scene, or later when adjusting the position of the balls as the cannon moves around.

    void compute(Mass& mass); // this is used to adjust the shapes position and velocity on screen. It is also where the ball will be computed to
    // 'bounce' when in collision with the sides of the game area.

    bool check_collision_with_flag_ball(Ball& ball); // this is used to determine if this ball is in collision with another ball in the scene. Then if either
    // ball has the 'group' flag set, then this is returned back as true, implying that the ball group flag needs to be spread. Thus the flag determining if there
    // is a group of the same balls is spread to all connected balls of the same colour.

    bool check_collision_with_anchor_ball(Ball& ball); // this is used to determine if there has been a collision with a ball that has the anchor flag. This
    // works fundamentally similar to the algorithm above, except this time there is no differentiation in result based on ball colour.

    void collision(Ball& ball, vector<Ball>& game_balls); // this is used to determine if any ball in the scene is in collision with any other ball in the scene,
    //  then, if there are two balls in collision, any potential velocity is stripped (stopping the ball fired from the cannon) and the balls are set back so they
    // appear to have arrived at the same angle, but no longer appear to overlap.

    void render(sf::RenderWindow& window); // this is used to render the ball to the scene.

    bool pop_sound_needs_playing(); // this is used to determine if the game should play a random pop sound. This condition is met when the ball is set to popped
    // but the pop sound hasn't yet been played.

    bool get_popped();
    float get_x_velocity();
    float get_y_velocity();
    float get_y_position();
    bool get_anchored();
    bool get_is_ball_to_fall();
    bool get_group_flag();
    // These getters all are used to allow private class attributes to be accessed from outside the class.

    void set_anchored(bool is_anchored);
    void set_ball_to_fall(bool is_to_fall);
    void set_group_flag(bool is_grouped);
    // These setters allow for private class attributes to be updated with new values, in doing so adjusting the behaviour of the balls
    // in the game level.
};

class Cannon {
    // This class is used to define the cannon, which is located at the bottom of the level, and is used to point and shoot balls
    // (defined earlier) into the game level. This is what the end user will interact with directly in each level.
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