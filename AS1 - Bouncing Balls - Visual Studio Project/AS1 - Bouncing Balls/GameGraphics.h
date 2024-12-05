#pragma once
#pragma warning( disable : 4244 )

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"

class Cloud {
private:
    float x_position;
    int y_position;
    float x_scale;
    float y_scale;
    float x_velocity;
    int transparency;
    sf::Texture cloud_texture;
    sf::Sprite cloud;
    bool is_rendered;
    bool loaded;
    string file_path;

    void generate_cloud();

public:
    Cloud();

    void compute();

    void render(sf::RenderWindow& window);

    void load();

    void unload();
};

class Ground {
private:
    sf::Texture grass_texture;
    sf::Sprite grass;

public:
    Ground();

    void compute();

    void render(sf::RenderWindow& window);
};

class LevelInstructions {
private:
    Text instructions_text[2];

    sf::RectangleShape bullet_mask[4];
    sf::CircleShape bullet[4];

    string raw_game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    string game_rules = "";
    int bullet_radius = 8;
    int bullet_size = bullet_radius * 2;
    int bullet_mask_size = bullet_size * 1.25;
    int game_start_area = (Registry::window_size[0] - Registry::window_size[1]) / 2;

public:
    LevelInstructions();

    void compute();

    void render(sf::RenderWindow& window);

    void load();

    void unload();
};

class MainMenuInstructions {
private:
    Text instructions_text[3];

    sf::RectangleShape bullet_mask[4];
    sf::CircleShape bullet[4];

    string raw_game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    string game_rules = "";
    int bullet_radius = 8;
    int bullet_size = bullet_radius * 2;
    int bullet_mask_size = bullet_size * 1.25;

public:
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

    sf::Texture dragon_texture;
    sf::Sprite dragon;

    string dragon_color;
    float dragon_scale;

public:
    Dragon();

    void compute(string position);

    void render(sf::RenderWindow& window);

    void load();

    void unload();
};