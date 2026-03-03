/*
This program is used to create the game specific graphics. This includes the Kawaii Dragon seen
on the left and right hand sides of the game window, as well as the clouds and ground seen in the background.
This is the program file, for the code structure found in the header file: 'GameGraphics.h'.
*/
#pragma warning( disable : 4305 )
// All the compiler warnings have been ignored because there where a large number of them, making it difficult to identify
// any actual problems.

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
    // The X and Y position for the clouds is generated randomly. The random X value ensures that the clouds don't all start
    // in one place, and then slowly separate out due to different velocities. The random Y value ensures that the clouds only appear
    // in the upper portions of the display, as these clouds - designed to represent Cumulus Humilis - do not form at low altitudes and the display
    // is meant to represent a view of the sky.

    float x_minimum_size = 50.0 / Registry::window_size[0];
    float x_maximum_size = 300.0 / Registry::window_size[0];
    float y_minimum_size = 50.0 / Registry::window_size[1];
    float y_maximum_size = 300.0 / Registry::window_size[1];
    // These are used to define the minimum and maximum sizes for the cloud in both dimensions, based on the window size - meaning that clouds would scale to match
    // the change is size of the window.

    x_scale = random_float(x_minimum_size, x_maximum_size);
    y_scale = random_float(y_minimum_size, y_maximum_size);
    // This instance of the cloud then is assigned a size between the min and max range in each dimension.

    x_velocity = (rand() % 1000) / 10000.0; // The X velocity is also randomized to ensure the clouds don't all move at the same speeds. This is intended
    // to create an additional sense of depth, as clouds closer appear to move faster in a real world environment.
 
    transparency = rand() % 200; // The transparency of the texture is also randomized to ensure the cloud always appears somewhat transparent and
    // ensures the clouds don't all look identical with the one provided texture.

    is_rendered = rand() % 2 == 1; // determines if this instance of the cloud texture is visible or not - remembering that clouds that aren't visible
    // don't have their textures rendered.
}

Cloud::Cloud() {
    string path_components[4] = { "resources",
            "images",
            "cloud.png" }; // The names of the file components needed to access the right texture relative to the current game location is
            // stored. This can then be converted into a path that is OS specific.

    file_path = path_builder(path_components);

    load(); // The cloud must always be loaded initially to ensure it is positioned and sized correctly, as whilst not visible, these clouds
    // still need to be processed to ensure that they don't get stuck forever in this state.
}

void Cloud::compute() {
    x_position += x_velocity; // The cloud x position changes with the velocity of the cloud each frame. The Y position for a cloud will always
    // remain constant.

    if (x_position > Registry::window_size[0]) {
        generate_cloud();
        int cloud_width = cloud.getGlobalBounds().width;
        x_position = -cloud_width;
        // This is used to reset the cloud when it is no longer visible on the right hand side of the window.
    }
    cloud.setPosition(x_position, y_position);
}

void Cloud::render(sf::RenderWindow& window) {
    if (is_rendered) {
        if (loaded == false) {
            // This ensures that if the cloud texture doesn't yet exist in RAM for this instance, it should be loaded to ensure
            // the cloud is correctly rendered on screen.
            load();
        }
        window.draw(cloud);
    }
    else {
        if (loaded) {
            // if the cloud is not supposed to be loaded, and is currently loaded, then it can be unloaded to reduce memory usage.
            unload();
        }
    }
}

void Cloud::load() {
    cloud_texture.loadFromFile(file_path);
    cloud.setTexture(cloud_texture);
    // This loads and associates the cloud texture as needed with the relevant sprite, in doing so also setting its size.

    generate_cloud(); // This cloud then gets its own unique appearance and position.

    cloud.setScale(x_scale, y_scale);
    cloud.setColor(sf::Color(255, 255, 255, transparency));
    // which is then reflected on the sprite its self

    loaded = true; // the cloud is only considered as loaded, after everything has been correctly loaded on-screen, to avoid any
    // potential threading problems.
}

void Cloud::unload() {
    loaded = false;

    sf::Texture new_cloud_texture; // Instead of loading a placeholder texture, a blank texture is created
    cloud.setTexture(new_cloud_texture); // all references to the original texture are removed
    cloud_texture = new_cloud_texture; // the old texture is removed from RAM, and the blank texture (which is considerably smaller) is
    // used instead.
}

Ground::Ground() {
    string path_components[4] = { "resources",
        "images",
        "cloud.png" };

    grass_texture.loadFromFile(path_builder(path_components));
    grass.setTexture(grass_texture);
    grass.setColor(sf::Color(213, 232, 212));
    grass.setScale(1.25, 0.1);
    // With the cloud, its appearance could change throughout the application run time. The ground operates on very similar principles to the
    // cloud, even sharing the texture, but does not change during run time, so has its initial values set once here instead.
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
    } // The text for the game instructions is repeated just enough so that it fits onto the window with only a small amount of excess

    for (int i = 0; i < 4; i++) {
        bullet[i].setRadius(bullet_radius);
        bullet[i].setFillColor(sf::Color::Black);
        bullet[i].setPointCount(calculate_point_count(bullet_radius));

        bullet_mask[i].setSize(sf::Vector2f(bullet_mask_size, bullet_mask_size));
        bullet_mask[i].setFillColor(Constants::SKY_BLUE);
    }
    // Each of the bullets, and their masks are set-up

    bullet_mask[0].setSize(sf::Vector2f(bullet_size, bullet_mask_size));
    bullet_mask[3].setSize(sf::Vector2f(bullet_size, bullet_mask_size));
    // change the size of the mask for the bottom 2 so that they don't hide the terrain behind them as noticeably.

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

    // Here each of the 4 bullets (and their corresponding masks) are positioned in the corners of the window, using the window size.
}

void MainMenuInstructions::compute() {
    // In this method, nothing needs computing, but was kept in for convention.
}

void MainMenuInstructions::render(sf::RenderWindow& window) {
    instructions_text[0].set_position(window, 0, Registry::window_size[1] - bullet_size); // left hand side
    instructions_text[0].set_rotation(-90);
    // The text for the left hand side is positioned and rotated so it faces inwards.

    instructions_text[1].set_position(window, bullet_size, 0); // top

    instructions_text[2].set_position(window, Registry::window_size[0], bullet_size); // right hand side
    instructions_text[2].set_rotation(90);
    // The text for the right hand side is positioned and rotated so it faces inwards.

    for (int i = 0; i < 3; i++) {
        instructions_text[i].render(window, game_rules, 14, sf::Color::Black, false);
    } // Each of the 3 sets of instructions are rendered to their correct positions on screen with the right formatting.

    for (int i = 0; i < 4; i++) {
        window.draw(bullet_mask[i]);
        window.draw(bullet[i]);
    } // Each of the bullet masks is then rendered, ensuring that the bullet and text dont touch. Then the bullet is rendered, neatly finishing
    // off the text, as it runs slightly off the screen.
}

void MainMenuInstructions::load() {
    for (int i = 0; i < 3; i++) {
        instructions_text[i].load();
    } // each of the instructions are loaded individually, with the text object handling the most work.
}

void MainMenuInstructions::unload() {
    for (int i = 0; i < 3; i++) {
        instructions_text[i].unload();
    } // each of the instructions are unloaded individually, with the text object doing the most work.
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
    // This is identical to the code for the MainMenuInstructions above, but with different positioning and quantities of text.
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
    // This is identical to the code for the MainMenuInstructions above, but with different positioning and quantities of text.
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
    // The colour of the dragon is randomly selected from the 3 ball colours used in this game. This is done in a similar process to the ball
    // colouring, except this time the result isn't a colour value, but a string used to uniquely identify each component texture.

    string path_components[4] = { "resources",
        "images",
        "kawaii",
        dragon_color + ".png"}; // the selected colour is then stored into this array of path components that can be constructed into a
        // fully qualified path for the target operating system.

    file_path = path_builder(path_components); // The path is then built using the operating specific path separator. It should be noted that this is
    // only done once, with the same file being loaded/unloaded, ensuring the dragon instance remains the right colour!

    dragon_scale = random_float(0.05, 0.12); // The dragon is then scaled between these two numerical values, giving them a bit of variety whilst ensuring
    // that they also fit between the window and the level border in the two game levels!
}

void Dragon::compute(string position) {
    if (loaded == false) {
        load();
    } // If the dragon hasn't yet been loaded, it needs to be loaded now, as whilst not rendered, it will have its size adjusted now which is
    // determined once the dragon texture has been assigned to the dragon sprite.
    float x_scale = dragon_scale;
    float y_scale = dragon_scale;

    if (position == Constants::LEFT) {
        x_scale *= -1;
    } // Scales the dragon based on its position on screen, ensuring that they always appear to spectate the game as it takes place!

    dragon.setScale(x_scale, y_scale);

    int dragon_width = dragon.getGlobalBounds().width;
    int dragon_height = dragon.getGlobalBounds().height;

    int x = dragon_width + Registry::ball_radius;

    if (position == Constants::RIGHT) {
        x = Registry::window_size[0] - (dragon_width + Registry::ball_radius);
    }
    // The x position of the dragon is calculated ensuring that it fits into the scene correctly, despite its adjusted size. This is also where
    // the position passed into the method takes effect.

    int y = Registry::window_size[1] - dragon_height; // The dragon is always rendered in the same vertical position on screen, as this dragon appears
    // to sit down. It was initially considered to have the dragons be able to take flight, but this was too complex to draw in time.

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
    dragon_texture.setSmooth(true); // For this load method the dragon texture is considered as 'smooth' before being assigned to the
    // relevant sprite. This was done to try and improve the quality of the sprite after it has been scaled.
    dragon.setTexture(dragon_texture);
    loaded = true;
}

void Dragon::unload() {
    loaded = false;
    sf::Texture new_dragon_texture;
    dragon.setTexture(new_dragon_texture);
    dragon_texture = new_dragon_texture;
}