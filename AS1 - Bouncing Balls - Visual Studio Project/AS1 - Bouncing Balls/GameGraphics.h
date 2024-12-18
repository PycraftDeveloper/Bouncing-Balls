/*
This program is used to create the game specific graphics. This includes the Kawaii Dragon seen
on the left and right hand sides of the game window, as well as the clouds and ground seen in the background.
This is the header file, and the corresponding code contents can be found in: 'GameGraphics.cpp'.
*/
#pragma once
#pragma warning( disable : 4244 )

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"
#include "UtilityObjects.h"

class Cloud {
    // This is used to render the clouds that appear in the background of the game. When instantiated
    // these don't belong to a specific level, and instead are handled centrally. Similarly, only 15 clouds
    // are ever created, then they are 're-cycled' through the duration of the game to avoid potential memory leaks.
private:
    float x_position;
    int y_position;
    // these two variables store the x and y position of the cloud in the background. Similar to the 'mass' GameObject, the
    // x_position has to be a float here as it is incremented by a small floating point number to create smooth motion across
    // the screen. By using an integer value like seen in the y_position, the clouds would not move across the sky!
    float x_scale;
    float y_scale;
    // these two variables store the scale of the cloud, in the x and y axis. Here the cloud image can be stretched and warped
    // to create more interesting looking clouds, instead of including lots of different cloud shapes as textures.
    float x_velocity; // this stores the velocity at which the cloud moves across the screen. This is the distance (in pixels)
    // per frame.
    int transparency; // this stores the transparency of the cloud. A cloud can be assigned a random transparency value once for the
    // duration of the transition across the screen, which can then be updated when the cloud is reset. This helps to make the cloud
    // appear more realistic and unique.
    sf::Texture cloud_texture;
    sf::Sprite cloud;
    bool is_rendered; // not all the clouds in the scene are rendered. This is to avoid creating too many clouds in the scene at any one
    // time. Clouds that are not rendered do NOT have their texture loaded into RAM.
    bool loaded; // used to determine if the cloud texture exists in RAM.
    string file_path; // the path to the texture file on the disk.

    void generate_cloud(); // this function is used to generate random attributes for the cloud when it is 'reset'. This is very important
    // as it primarily is used to hide the fact that clouds are re-used repeatedly in the game.

public:
    Cloud();

    void compute(); // used to update the clouds horizontal position on screen, and also determines if the cloud is to be 'reset'.

    void render(sf::RenderWindow& window); // renders the cloud to the window.

    void load(); // used to load the cloud texture from disk, provided that it hasn't been loaded already. This also causes the cloud to
    // 'reset' to approach the problem first identified in the 'GamObjects' code where the sprite will maintain its size if the texture
    // assigned doesn't change in size. Failing to do this would lead to clouds appearing impossibly small in the scene.

    void unload(); // used to remove the cloud texture from RAM, freeing up memory when the cloud isn't being rendered.
};

class Ground {
    // Used to create the basic ground seen running along the bottom of the game.
private:
    sf::Texture grass_texture; // The grass texture is actually just the cloud texture, but has been re-used and re-coloured to reduce the
    // overall size of the game. If the grass had more importance in the game, it would likely be re-textured.
    sf::Sprite grass;

public:
    Ground();

    void compute();

    void render(sf::RenderWindow& window); // renders the grass to the window.
};

class LevelInstructions {
    // This is used to render the text that constrains the game level to a square. This text holds the instructions seen in the original
    // 'Puzzle Bobble' game by Taito.
private:
    Text instructions_text[2]; // Stores the individual text objects in an array. One element for each side of the level area.

    sf::RectangleShape bullet_mask[4]; // stores the four masks used to hide the content directly behind the bullets. This is used to mask any
    // further repetitions of the text and helps to make the game look better.
    sf::CircleShape bullet[4]; // the bullets, an abbreviation of 'bullet points' are used to define breaks in the text, and are used to hide 
    // the fact that the text might not be the right size for the window, and extend off-screen slightly.

    string raw_game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!"; // This is the text that the game renders, taken from the original game.
    string game_rules = ""; // This is used to store the raw game rules, but repeated just enough times to be able to fit on-screen.
    int bullet_radius = 8; // This represents the radius of the black circles used as bullets in the game text. The bullets should ideally be
    // approximately the same diameter as the text appears tall.
    int bullet_size = bullet_radius * 2; // This is used to store the diameter of the bullet circle, using the equation d = 2r
    int bullet_mask_size = bullet_size * 1.25; // This is used to determine how far the mask behind the bullets should extend (in pixels) from the centre
    // of the bullet. This is important to ensure the text doesn't touch the bullets, however making this too large would make the fact that small amounts of the
    // background get cut into sightly by the bullet mask.
    int game_start_area = (Registry::window_size[0] - Registry::window_size[1]) / 2; // This is used to determine the start position for the level area
    // In game, the level area is a square, and this is determined by taking the window height to be it's smallest size, and adjusting the game area accordingly.
    // This was another problem faced in getting the window to resize, in that the game area changes in location and size, causing major problems that where not
    // solved before the deadline.

public:
    LevelInstructions();

    void compute(); // Used to place the text and bullets around the level area. They are placed such that the inner edge of the square it creates with the top
    // and bottom of the window represent the barrier for the game area, that no game object can exceed. The text is also rotated so that it faces into the level area.

    void render(sf::RenderWindow& window); // Used to render the text and bullet masks and bullets to the screen. This is one of a few times where the actual render
    // call is neater than just accessing the sprite from the class and rendering it. However it has been observed in more complex games that this approach is better.

    void load(); // This class does not actually load any textures from disk, but instead this is passed into the text objects to control their loading. It was initially
    // considered to have the bullets as their own texture, but this was later replaced with circle shapes to reduce memory usage and overall game size.

    void unload(); // This is passed through to the two text objects and triggers their unload methods.
};

class MainMenuInstructions {
    // This class is very similar to the class above, except this time it is used to render the game instructions on the main menu and pause menu.
    // Furthermore, unlike the previous class, this class renders text on the right and left hand sides of the window, and along the top instead of the
    // decorating the level area border. Whilst these two classes are similar in purpose, they couldn't both be inherited easily from a single class
    // as there are different numbers of objects, and they are placed in different positions, which is why they exist separately.
    // In this class=, only the differences between the two classes have been commented on.
private:
    Text instructions_text[3]; // Stores the individual text objects in an array. One for the top, and then one for each side.

    sf::RectangleShape bullet_mask[4];
    sf::CircleShape bullet[4];

    string raw_game_rules = "AIM AT BUBBLES OF THE SAME COLOR AND SHOOT!";
    string game_rules = "";
    int bullet_radius = 8;
    int bullet_size = bullet_radius * 2;
    int bullet_mask_size = bullet_size * 1.25;

    // Note that unlike the class above, the position of the game area is irrelevant here because the objects are positioned around the window,
    // rather than around a specific area.

public:
    MainMenuInstructions();

    void compute(); // This method is in charge of handling the positions of all the text, bullets and bullet masks relative to the window
    // and also ensures that the text on the sides is rotated again to face inwards.

    void render(sf::RenderWindow& window); // This method is used to draw the text, bullet masks and bullets (in that order) to the window.

    void load();

    void unload();
};

class Dragon {
    // This is used to render the dragons that appear on the left and right hand sides of most levels. They do not appear on the pause menu as
    // the idea is the pause menu can be switched between very quickly, and slowing this transition down to load the dragons wasn't necessary and might
    // detract from its purpose.
private:
    bool loaded = false; // used to determine if the dragon texture exists in RAM.
    string file_path; // used to identify the full location on the disk for the dragon texture - bearing in mind there are 3 different
    // varieties, one for each colour of the balls in the game.

    sf::Texture dragon_texture;
    sf::Sprite dragon;

    string dragon_color; // This is used to select which colour dragon to render. Each dragon instance has its own assigned colour.
    float dragon_scale; // This is used to vary the size of the dragon. The dragon texture is square so only one value is needed. This allows for
    // the varying dragon sizes, adding to the illusion that there are more dragons than there are textures.

public:
    Dragon(); // here the dragon's colour is randomly selected, as also is their size. The dragon's size must be within a very exact range such that it appears
    // perfectly in the gap between the window and the level border.

    void compute(string position); // This is used to position the dragon on screen. The position refers to the constants, LEFT and RIGHT which this method
    // can then interpret into numerical positions on the window's coordinate system.

    void render(sf::RenderWindow& window); // this is used to render the dragon to the window.

    void load(); // this is used to load the dragon from disk if it doesn't already exist in RAM

    void unload(); // this is used to remove the dragon texture and all references to it from RAM when it is no longer needed, freeing up space.
};