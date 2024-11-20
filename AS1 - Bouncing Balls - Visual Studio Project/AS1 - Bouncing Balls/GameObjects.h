#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"

class Mass {
public:
    sf::Texture mass_texture;
    sf::Sprite mass;
    float x_position;
    float y_position;
    float mass_sprite_x_size;
    float mass_sprite_y_size;
    float vertical_offset = 0;

    Mass();

    int get_game_ceiling();

    void compute(sf::RenderWindow& window);

    void render(sf::RenderWindow& window);

    void reset();
};

class Ball {
public:
    float x, y;
    float radius;
    sf::Color color;
    float shape_x_velocity = 0;
    float shape_y_velocity = 0;
    sf::CircleShape shape;
    bool popped = false;

    Ball(
        float x,
        float y);

    void set_velocity(float x_velocity, float y_velocity);

    void set_position(float new_x, float new_y);

    void compute(Mass& mass);

    void collision(Ball& ball);

    void render(sf::RenderWindow& window);
};

class Cannon {
public:
    sf::Texture cannon_texture;
    sf::Sprite cannon;
    float rotation = 0;
    int unrotated_cannon_height = 0;
    float cannon_x_position = 0;
    float cannon_y_position = 0;

    Cannon(vector<Ball>& game_balls);

    void set_position(float new_x, float new_y, vector<Ball>& game_balls);

    void load_cannon_with_ball(vector<Ball>& game_balls);

    void compute(sf::RenderWindow& window, PlayerInput& player_input, vector<Ball>& game_balls);

    float get_rotation();

    void render(sf::RenderWindow& window);
};