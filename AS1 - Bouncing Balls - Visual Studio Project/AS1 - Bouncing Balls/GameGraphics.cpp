#include <SFML/Graphics.hpp>
#include <string>

#include "GameGraphics.h"
#include "Registry.h"
#include "UtilityFunctions.h"

using namespace std;

void Cloud::generate_cloud() {
    x_position = rand() % Registry::window_size[0];
    y_position = rand() % Registry::window_size[1] / 5.0;

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
    string path_components[50] = { "resources",
            "images",
            "cloud.png" };

    cloud_texture.loadFromFile(path_builder(path_components));
    cloud.setTexture(cloud_texture);

    generate_cloud();

    cloud.setScale(x_scale, y_scale);
    cloud.setColor(sf::Color(255, 255, 255, transparency));
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
        window.draw(cloud);
    }
}