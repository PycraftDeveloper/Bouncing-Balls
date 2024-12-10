/*
Welcome to the Bouncing Balls code!

Here is a list of what each component does:
    * Constants - Stores a global collection of constants.
    * Registry - Stores a global collection of variables.
    * GameGraphics - Stores the code needed to produce all the non-critical game graphical content.
    * GameMenus - Stores the code needed to display and interact with each menu and level in the game.
    * GameObjects - Stores the code needed for the game specific objects used in the game levels.
    * UtilityObjects - Stores the code needed for general UI development and not game specific.
    * UtilityFunctions - Stores a lot of the helper code that is needed in a lot of places throughout the game.
    * main - This is the most important bit, handling user inputs, the movement through levels as well as content that exists across all levels.

Available inputs:
    * The 'Esc' key on the main menu closes the game.
    * The 'Esc' key in a level pauses the game.
    * The 'Esc' key in the pause menu returns you to the level.
    * The 'Esc' key in the end menu returns you to the main menu.
    * You can use the left click mouse button to fire the cannon in a level.
    * You can also use the left click mouse button to interact with buttons in game.
    * You can use the 'Enter' key for the exact same functionality as the left click mouse button.

Each program in this project has been given an overview which discusses in more detail its purpose. Additionally
for header files, focus has been put on why that component is necessary, and an overview of its purpose, whereas in the
'.cpp'files a focus has been put on why the functionality has been written in this way as to avoid duplicating comments.
*/

// include external libraries (third party content)
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

// include internal libraries (first party content)
#include "GameObjects.h"
#include "GameMenus.h"
#include "GameGraphics.h"
#include "UtilityObjects.h"
#include "UtilityFunctions.h"
#include "Constants.h"
#include "Registry.h"

int main()
{
    srand(time(0)); // enforces random values by using a seed based on the current time

    // Create and format the Window here - resizing has been disabled due to complications with the ball
    // arrangement for the game levels.
    sf::RenderWindow window;
    window.create(
        sf::VideoMode(Registry::window_size[0], Registry::window_size[1]),
        "Bouncing Balls!",
        sf::Style::Titlebar | sf::Style::Close);

    window.setFramerateLimit(60); // Prevent the game from using too much CPU resources by limiting how
    // often the display refreshes.

    // set-up local variables (start)
    string menu_navigation[] = { Constants::MAIN_MENU, Constants::MAIN_MENU }; // The Menu Navigation System, created in the variable
    // menu_navigation, allows me to easily move between the current menu and the previous menu - so that the player can return to 
    // the main menu at any time or a pause menu and the game can "remember" what was happening before this switch occurred.
    bool game_running = true; // instead of a conventional game loop, using a boolean variable here
    // allows for the game loop to be exited via multiple methods:
    // 1) By clicking the 'X' in the top-right of the window
    // 2) By pressing 'Esc' key on the main menu
    // 3) By clicking the 'Quit' button on the main menu, pause menu or end menu
    string next_component; // This allows for comparisons to trigger events when changing menus. This should always be the same,
    // as the content stored in menu_navigation[0] but will be used to trigger events when these don't match.
    float main_theme_fade_start = -1; // this is used to store the time (because time is continuous, a float has been used for improved
    // accuracy) that the main theme begins to fade out.
    bool window_focus = true; // stores a boolean value used to determine if the window is in focus or not. This is updated once per frame instead of
    // repeatedly calling 'window.getFocus()' in the same game loop as this is unnecessary. By default the window is created in focus so is set to true.
    int current_volume = 15; // stores the volume of the main theme music when it starts playing. This will be updated to reflect the volume of the game
    // as it is at the moment when the window focus changes (which is used for the main menu volume adjustment).
    // set-up local variables (end)

    // This is used to distribute the user input through each level and combines different valid inputs into one which makes checking simpler. 
    PlayerInput player_input;

    // instantiate the game menus, there will only ever be one instance of each menu in the game.
    MainMenu main_menu;
    LevelOne level_one;
    LevelTwo level_two;
    GameEndMenu end_menu;
    PauseMenu pause_menu;

    // 'initialize' the game levels. This is NOT the class constructor, but is instead used to prepare the level objects to be used.
    level_one.init();
    level_two.init();

    // instantiate the game objects, these have no role in defining game mechanics, but instead are stylistic choices.
    Cloud clouds[15];
    Ground ground;

    // prepare the main theme for the game and load it immediately before going into the game loop. This will loop throughout the
    // entirety of the game.
    sf::Music main_theme;
    string path_components[4] = { "resources", "music", "main theme [extended].ogg"};
    main_theme.openFromFile(path_builder(path_components));
    main_theme.setLoop(true);
    main_theme.setVolume(15);
    main_theme.play();

    while (game_running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                game_running = false;
                break;
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Enter:
                    player_input.set_player_button_input(true); // when the user presses the 'Enter' key, this is stored simply as
                    // the player primary input. This is a simple game and only one input is realistically needed, so different
                    // methods of considering this input are aggregated together.
                    break;
                case sf::Keyboard::Escape: // When the player presses the 'Esc' key, the interactions detailed earlier ALL occur here.
                    if (menu_navigation[0] == Constants::PAUSE_MENU) {
                        // return from pause menu to current level.
                        menu_navigation[0] = menu_navigation[1];
                        menu_navigation[1] = Constants::PAUSE_MENU;
                        pause_menu.unload();
                    } else if (menu_navigation[0] == Constants::LEVEL_ONE || menu_navigation[0] == Constants::LEVEL_TWO) {
                        // go to the pause menu from the current menu.
                        menu_navigation[1] = menu_navigation[0];
                        menu_navigation[0] = Constants::PAUSE_MENU;
                    }
                    else if (menu_navigation[0] == Constants::MAIN_MENU) {
                        // exit the game if on the main menu
                        menu_navigation[0] = Constants::QUIT;
                    }
                    else if (menu_navigation[0] == Constants::END_MENU) {
                        // go to the main menu if on the end menu
                        menu_navigation[1] = menu_navigation[0];
                        menu_navigation[0] = Constants::MAIN_MENU;
                    }
                    break;
                }
            }
        }

        // Window focus main theme volume adjustment (start)
        // When the window looses focus, the main theme lowers from 15% volume to 5% volume to give the illusion of being of being 'in the background'.
        // When the window regains focus, the reverse is also true.
        if (window.hasFocus() != window_focus) {
            // when the focus changes, get the current run time as the starting time, and the current volume.
            // getting the current volume ensures that if the window gains/looses focus repeatedly, the transition
            // remains smooth and doesn't jump to the min/max values.
            window_focus = window.hasFocus();
            main_theme_fade_start = Registry::run_time;
            current_volume = main_theme.getVolume();
        }

        if (window_focus == false) {
            // fade the music out
            main_theme.setVolume(interpolate_value(current_volume, 5, 1, Registry::run_time - main_theme_fade_start));
        }
        else {
            // fade the music in
            main_theme.setVolume(interpolate_value(current_volume, 15, 1, Registry::run_time - main_theme_fade_start));
        }
        // Window focus main theme volume adjustment (end)

        // If the window is out of focus, the game goes to the pause menu when in the level. This prevents the player from loosing the level when
        // they are doing something else on the machine.
        if (window_focus == false) {
            if (menu_navigation[0] == Constants::LEVEL_ONE || menu_navigation[0] == Constants::LEVEL_TWO) {
                menu_navigation[1] = menu_navigation[0];
                menu_navigation[0] = Constants::PAUSE_MENU;
            }
        }

        window.clear(Constants::SKY_BLUE);

        for (int i = 0; i < 15; i++) {
            // compute and render each of the clouds. These are drawn first meaning they appear in the background, effectively forming the 'sky box' for the game in 2D.
            clouds[i].compute();
            clouds[i].render(window);
        }

        // compute and render the ground.
        ground.compute();
        ground.render(window);

        // menu navigation (start)
        // This next block of code is used to control the navigation between each menu and level in the game.
        // Each menu ends its 'run menu' function by determining where the game should go to next, it is not the
        // job of this block of code then to determine where to go next from each level, but to consider what needs to be
        // done in order to get there - resetting levels, unloading data ext.

        if (menu_navigation[0] == Constants::MAIN_MENU) {
            // when rendering the main menu it is important to consider resetting the game score to zero, as getting here effectively
            // means all progress in the levels has been lost.
            Registry::score = 0;
            next_component = main_menu.run_menu(window, player_input);
            if (next_component != Constants::MAIN_MENU) {
                // if the next component is not the main menu, the levels need to be reset and the main menu can be unloaded.
                // when unloading something, the textures for the object are removed from memory to save on space in RAM.
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::MAIN_MENU;
                // the menu_navigation bit here is important. This will overwrite the second element of the menu_navigation array with
                // the current menu and set the first element to the next menu to use. This ensures on the next iteration of the game loop,
                // another menu is instead used, and if the previous menu needed to be returned to (for example in the pause menu) then the second element
                // in this array stores this information.
                main_menu.unload();
                level_one.reset_level();
                level_two.reset_level();
            }
        }
        else if (menu_navigation[0] == Constants::LEVEL_ONE) {
            next_component = level_one.run_menu(window, player_input, Constants::LEVEL_ONE);
            if (next_component != Constants::LEVEL_ONE) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::LEVEL_ONE;
                level_one.unload();
                if (next_component != Constants::PAUSE_MENU) {
                    // If the next component to go to is not the current one, or the pause menu then just this level gets reset.
                    // This means that by the time you each level two, level one has already been reset for the next attempt.
                    level_one.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == Constants::LEVEL_TWO) {
            next_component = level_two.run_menu(window, player_input, Constants::LEVEL_TWO);
            if (next_component != Constants::LEVEL_TWO) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::LEVEL_TWO;
                level_two.unload();
                if (next_component != Constants::PAUSE_MENU) {
                    level_two.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == Constants::END_MENU) {
            next_component = end_menu.run_menu(window, player_input);

            if (next_component != Constants::END_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::END_MENU;
                end_menu.unload();
            }
        }
        else if (menu_navigation[0] == Constants::PAUSE_MENU) {
            next_component = pause_menu.run_menu(
                window,
                menu_navigation,
                player_input); // this call is different from the others.
                // For the others, the window is passed through as a render target, and the player input is passed in
                // to ensure the user can interact with the level. For this menu the menu navigation array is also passed in
                // as this is used by the menu to determine which menu to go to next when the user clicks the 'resume game' button

            if (next_component != Constants::PAUSE_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::PAUSE_MENU;
                pause_menu.unload();

                if (next_component == Constants::MAIN_MENU) {
                    level_one.unload();
                    level_two.unload();
                    end_menu.unload();
                }
            }
        }
        else if (menu_navigation[0] == Constants::QUIT) {
            // There is no quit menu, instead this just triggers the game loop to exit after completing this frame.
            game_running = false;
        }
        // menu navigation (end)

        window.display();

        player_input.update(window); // Here, the player input is updated, and this is used to reset the player input and to
        // update the mouse position, which is used for cannon targeting and for button interactions.

        Registry::run_time += 1 / 60.0; // Because this game is frame rate limited, and not complex enough to continually run at a slower refresh rate
        // the time taken to run a single frame can be used instead of keeping track of the actual system time at the start and end of the game loop. This
        // approach is less flexible than this, but works in this environment. The game occasionally doesn't reach the 60-FPS target when loading in resources,
        // however this generally only occurs when transitioning between levels so doesn't detract from the games overall smoothness.
    }

    return 0;
}