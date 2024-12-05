#pragma warning( disable : 4305 )

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "GameGraphics.h"
#include "Registry.h"
#include "UtilityFunctions.h"

using namespace std;

void Cloud::generate_cloud() {
    x_position = rand() % Registry::window_size[0];
    y_position = rand() % Registry::window_size[1] / 5;

    float x_minimum_size = 50.0 / Registry::window_size[0];
    float x_maximum_size = 300.0 / Registry::window_size[0];
    float y_minimum_size = 50.0 / Registry::window_size[1];
    float y_maximum_size = 300.0 / Registry::window_size[1];

    x_scale = random_float(x_minimum_size, x_maximum_size);
    y_scale = random_float(y_minimum_size, y_maximum_size);
    x_velocity = (rand() % 1000) / 10000.0;
    transparency = rand() % 200;
    is_rendered = rand() % 2 == 1;
}

Cloud::Cloud() {
    string path_components[4] = { "resources",
            "images",
            "cloud.png" };

    file_path = path_builder(path_components);

    load();
}

void Cloud::compute() {
    x_position += x_velocity;

    if (x_position > Registry::window_size[0]) {
        generate_cloud();
        int cloud_width = cloud.getGlobalBounds().width;
        x_position = -cloud_width;
    }
    cloud.setPosition(x_position, y_position);
}

void Cloud::render(sf::RenderWindow& window) {
    if (is_rendered) {
        if (loaded == false) {
            load();
        }
        window.draw(cloud);
    }
    else {
        if (loaded) {
            unload();
        }
    }
}

void Cloud::load() {
    cloud_texture.loadFromFile(file_path);
    cloud.setTexture(cloud_texture);

    generate_cloud();

    cloud.setScale(x_scale, y_scale);
    cloud.setColor(sf::Color(255, 255, 255, transparency));
    loaded = true;
}

void Cloud::unload() {
    loaded = false;
    sf::Texture new_cloud_texture;
    cloud.setTexture(new_cloud_texture);
    cloud_texture = new_cloud_texture;
}

Ground::Ground() {
    string path_components[4] = { "resources",
        "images",
        "cloud.png" };

    grass_texture.loadFromFile(path_builder(path_components));
    grass.setTexture(grass_texture);
    grass.setColor(sf::Color(213, 232, 212));
    grass.setScale(1.25, 0.1);
}

void Ground::compute() {
    int x_position = (Registry::window_size[0] - grass.getGlobalBounds().width) / 2;
    int y_position = Registry::window_size[1] - 30;
    // used 30 so that the cloud texture can appear as a solid ground.
    grass.setPosition(x_position, y_position);
}

void Ground::render(sf::RenderWindow& window) {
    window.draw(grass);
}

MainMenuInstructions::MainMenuInstructions() {
    for (int i = 0; i < 5; i++) {
        game_rules += " " + raw_game_rules;
    }
    for (int i = 0; i < 4; i++) {
        bullet[i].setRadius(bullet_radius);
        bullet[i].setFillColor(sf::Color::Black);
        bullet[i].setPointCount(calculate_point_count(bullet_radius));

        bullet_mask[i].setSize(sf::Vector2f(bullet_mask_size, bullet_mask_size));
        bullet_mask[i].setFillColor(Constants::SKY_BLUE);
    }

    bullet_mask[0].setSize(sf::Vector2f(bullet_size, bullet_mask_size));
    bullet_mask[3].setSize(sf::Vector2f(bullet_size, bullet_mask_size));
    // change the size of the mask for the bottom 2 so that they dont hide the terrain behind them!

    bullet[0].setPosition(0, Registry::window_size[1] - bullet_size);
    bullet_mask[0].setPosition(0, Registry::window_size[1] - bullet_mask_size);

    bullet[1].setPosition(0, 0);
    bullet_mask[1].setPosition(0, 0);

    bullet[2].setPosition(Registry::window_size[0] - bullet_size, 0);
    bullet_mask[2].setPosition(Registry::window_size[0] - bullet_mask_size, 0);

    bullet[3].setPosition(Registry::window_size[0] - bullet_size, Registry::window_size[1] - bullet_size);
    bullet_mask[3].setPosition(Registry::window_size[0] - bullet_size, Registry::window_size[1] - bullet_mask_size);
    // this bottom one is different to reflect the position change due to the size change made earlier, the other one [0]
    // doesn't need this change as its x position is zero.
}

void MainMenuInstructions::compute() {
}

void MainMenuInstructions::render(sf::RenderWindow& window) {
    instructions_text[0].set_position(window, 0, Registry::window_size[1] - bullet_size); // left hand side
    instructions_text[0].set_rotation(-90);

    instructions_text[1].set_position(window, bullet_size, 0); // top

    instructions_text[2].set_position(window, Registry::window_size[0], bullet_size); // right hand side
    instructions_text[2].set_rotation(90);

    for (int i = 0; i < 3; i++) {
        instructions_text[i].render(window, game_rules, 14, sf::Color::Black, false);
    }

    for (int i = 0; i < 4; i++) {
        window.draw(bullet_mask[i]);
        window.draw(bullet[i]);
    }
}

void MainMenuInstructions::load() {
    for (int i = 0; i < 3; i++) {
        instructions_text[i].load();
    }
}

void MainMenuInstructions::unload() {
    for (int i = 0; i < 3; i++) {
        instructions_text[i].unload();
    }
}

LevelInstructions::LevelInstructions() {
    for (int i = 0; i < 5; i++) {
        game_rules += " " + raw_game_rules;
    }
    for (int i = 0; i < 4; i++) {
        bullet[i].setRadius(bullet_radius);
        bullet[i].setFillColor(sf::Color::Black);
        bullet[i].setPointCount(calculate_point_count(bullet_radius));

        bullet_mask[i].setSize(sf::Vector2f(bullet_size, bullet_mask_size));
        bullet_mask[i].setFillColor(Constants::SKY_BLUE);
    }

    bullet[0].setPosition(game_start_area - bullet_size, Registry::window_size[1] - bullet_size);
    bullet_mask[0].setPosition(game_start_area - bullet_size, Registry::window_size[1] - bullet_mask_size);

    bullet[1].setPosition(game_start_area - bullet_size, 0);
    bullet_mask[1].setPosition(game_start_area - bullet_size, 0);

    bullet[2].setPosition(game_start_area + Registry::window_size[1], 0);
    bullet_mask[2].setPosition(game_start_area + Registry::window_size[1], 0);

    bullet[3].setPosition(game_start_area + Registry::window_size[1], Registry::window_size[1] - bullet_size);
    bullet_mask[3].setPosition(game_start_area + Registry::window_size[1], Registry::window_size[1] - bullet_mask_size);
}

void LevelInstructions::compute() {
}

void LevelInstructions::render(sf::RenderWindow& window) {
    instructions_text[0].set_position(window, game_start_area - bullet_size, Registry::window_size[1] - bullet_size); // left hand side
    instructions_text[0].set_rotation(-90);

    instructions_text[1].set_position(window, Registry::window_size[1] + game_start_area + bullet_size, bullet_size); // right hand side
    instructions_text[1].set_rotation(90);

    for (int i = 0; i < 2; i++) {
        instructions_text[i].render(window, game_rules, 14, sf::Color::Black, false);
    }

    for (int i = 0; i < 4; i++) {
        window.draw(bullet_mask[i]);
        window.draw(bullet[i]);
    }
}

void LevelInstructions::load() {
    for (int i = 0; i < 2; i++) {
        instructions_text[i].load();
    }
}

void LevelInstructions::unload() {
    for (int i = 0; i < 2; i++) {
        instructions_text[i].unload();
    }
}

Dragon::Dragon() {
    int randomly_chosen_color = rand() % 3;

    if (randomly_chosen_color == 0) {
        dragon_color = "blue";
    }
    else if (randomly_chosen_color == 1) {
        dragon_color = "yellow";
    }
    else {
        dragon_color = "red";
    }

    string path_components[4] = { "resources",
        "images",
        "kawaii",
        dragon_color + ".png"};

    file_path = path_builder(path_components);

    dragon_scale = random_float(0.05, 0.12);
}

void Dragon::compute(string position) {
    if (loaded == false) {
        load();
    }
    float x_scale = dragon_scale;
    float y_scale = dragon_scale;

    if (position == Constants::LEFT) {
        x_scale *= -1;
    }

    dragon.setScale(x_scale, y_scale);

    int dragon_width = dragon.getGlobalBounds().width;
    int dragon_height = dragon.getGlobalBounds().height;

    int x = dragon_width + Registry::ball_radius;

    if (position == Constants::RIGHT) {
        x = Registry::window_size[0] - (dragon_width + Registry::ball_radius);
    }

    int y = Registry::window_size[1] - dragon_height;

    dragon.setPosition(x, y);
}

void Dragon::render(sf::RenderWindow& window) {
    if (loaded == false) {
        load();
    }
    window.draw(dragon);
}

void Dragon::load() {
    dragon_texture.loadFromFile(file_path);
    dragon_texture.setSmooth(true);
    dragon.setTexture(dragon_texture);
    loaded = true;
}

void Dragon::unload() {
    loaded = false;
    sf::Texture new_dragon_texture;
    dragon.setTexture(new_dragon_texture);
    dragon_texture = new_dragon_texture;
}