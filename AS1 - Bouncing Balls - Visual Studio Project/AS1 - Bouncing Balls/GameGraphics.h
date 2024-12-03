#pragma once
#pragma warning( disable : 4244 )

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"

class Cloud {
    float x_position;
    int y_position;
    float x_scale;
    float y_scale;
    float x_velocity;
    int transparency;
    sf::Texture cloud_texture;
    sf::Sprite cloud;
    bool is_rendered;
public:
    void generate_cloud();

    Cloud();

    void compute();

    void render(sf::RenderWindow& window);
};

class Ground {
public:
    sf::Texture grass_texture;
    sf::Sprite grass;
    Ground();

    void compute();

    void render(sf::RenderWindow& window);
};

class LevelInstructions {
public:
    Text instructions_text[2];

    sf::RectangleShape bullet_mask[4];
    sf::CircleShape bullet[4];

    string raw_game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    string game_rules = "";
    int bullet_radius = 8;
    int bullet_size = bullet_radius * 2;
    int bullet_mask_size = bullet_size * 1.25;
    int game_start_area = (Registry::window_size[0] - Registry::window_size[1]) / 2;

    LevelInstructions();

    void compute();

    void render(sf::RenderWindow& window);

    void load();

    void unload();
};

class MainMenuInstructions {
public:
    Text instructions_text[3];

    sf::RectangleShape bullet_mask[4];
    sf::CircleShape bullet[4];

    string raw_game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    string game_rules = "";
    int bullet_radius = 8;
    int bullet_size = bullet_radius * 2;
    int bullet_mask_size = bullet_size * 1.25;

    MainMenuInstructions();

    void compute();

    void render(sf::RenderWindow& window);

    void load();

    void unload();
};

class Dragon {
private:
    bool loaded = false;
    string file_path;
public:
    sf::Texture dragon_texture;
    sf::Sprite dragon;

    string dragon_color;
    float dragon_scale;

    Dragon();

    void compute(string position);

    void render(sf::RenderWindow& window);

    void load();

    void unload();
};