#pragma once

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"

class Cloud {
    float x_position;
    float y_position;
    float x_scale;
    float y_scale;
    float x_velocity;
    float transparency;
    sf::Texture cloud_texture;
    sf::Sprite cloud;
public:
    void generate_cloud();

    Cloud();

    void compute();

    void render(sf::RenderWindow& window);
};