#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
#include <vector>

#include "GameObjects.h"
#include "GameMenus.h"
#include "GameGraphics.h"
#include "UtilityObjects.h"
#include "UtilityFunctions.h"
#include "Constants.h"
#include "Registry.h"

int main()
{
    srand(time(0)); // enforces random values using seed of current time.

    sf::RenderWindow window;
    window.create(
        sf::VideoMode(Registry::window_size[0], Registry::window_size[1]),
        "Bouncing Balls!",
        sf::Style::Titlebar | sf::Style::Close);

    window.setVerticalSyncEnabled(true); // v-sync is our frame limiter here.
    // No need for clocks!

    // set-up local variables
    string menu_navigation[] = { Constants::MAIN_MENU, Constants::MAIN_MENU };
    /*
    * The Menu Navigation System, created in the variable menu_navigation, allows us
    * to easily move between the current menu and the previous menu - so that the
    * player can return to the main menu at any time or a pause menu and the game can
    * "remember" what was happening before this switch occurred. In a larger game
    * the array could be larger as there are more menus to navigate through,
    * but I am keeping this relatively simple and small.
    */
    int score = 0;
    bool window_full_screen = false;
    bool game_running = true;

    string next_component;

    // set-up game componentry
    PlayerInput player_input;

    // set-up game levels
    MainMenu main_menu;
    LevelOne level_one;
    LevelTwo level_two;
    GameEndMenu end_menu;
    PauseMenu pause_menu;

    level_one.init();
    level_two.init();

    Cloud clouds[15];
    Ground ground;

    sf::Music main_theme;
    string path_components[4] = { "resources", "music", "main theme [extended].ogg"};
    main_theme.openFromFile(path_builder(path_components));
    main_theme.setLoop(true);
    main_theme.setVolume(15);
    //main_theme.play();

    float main_theme_fade_start = -1;
    float main_theme_fade_current = 0;
    bool window_focus = true;
    int current_volume = 15;

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
                    player_input.set_player_button_input(true);
                    break;
                case sf::Keyboard::Escape:
                    if (menu_navigation[0] == Constants::PAUSE_MENU) {
                        menu_navigation[0] = menu_navigation[1];
                        menu_navigation[1] = Constants::PAUSE_MENU;
                        pause_menu.unload();
                    } else if (menu_navigation[0] == Constants::LEVEL_ONE || menu_navigation[0] == Constants::LEVEL_TWO) {
                        menu_navigation[1] = menu_navigation[0];
                        menu_navigation[0] = Constants::PAUSE_MENU;
                        // note, this is flipped compared to above because exiting the pause menu
                    }
                    else if (menu_navigation[0] == Constants::MAIN_MENU) {
                        menu_navigation[0] = Constants::QUIT;
                    }
                    else if (menu_navigation[0] == Constants::END_MENU) {
                        menu_navigation[1] = menu_navigation[0];
                        menu_navigation[0] = Constants::MAIN_MENU;
                    }
                    break;
                case sf::Keyboard::Num1: // [temporary] forced switching between levels
                    menu_navigation[0] = Constants::MAIN_MENU;
                    menu_navigation[1] = Constants::MAIN_MENU;
                    break;
                case sf::Keyboard::Num2: // [temporary] forced switching between levels
                    menu_navigation[0] = Constants::LEVEL_ONE;
                    menu_navigation[1] = Constants::LEVEL_ONE;
                    break;
                case sf::Keyboard::Num3: // [temporary] forced switching between levels
                    menu_navigation[0] = Constants::LEVEL_TWO;
                    menu_navigation[1] = Constants::LEVEL_TWO;
                    break;
                case sf::Keyboard::Num4: // [temporary] forced switching between levels
                    menu_navigation[0] = Constants::END_MENU;
                    menu_navigation[1] = Constants::END_MENU;
                    break;
                case sf::Keyboard::Num5: // [temporary] forced switching between levels
                    menu_navigation[0] = Constants::PAUSE_MENU;
                    menu_navigation[1] = Constants::PAUSE_MENU;
                    break;
                }
            }
        }

        if (window.hasFocus() != window_focus) {
            window_focus = window.hasFocus();
            main_theme_fade_start = Registry::run_time;
            current_volume = main_theme.getVolume();
        }

        if (window_focus == false) {
            main_theme_fade_current = Registry::run_time;
            main_theme.setVolume(interpolate_value(current_volume, 5, 1, main_theme_fade_current - main_theme_fade_start));

            if (menu_navigation[0] == Constants::LEVEL_ONE || menu_navigation[0] == Constants::LEVEL_TWO) {
                menu_navigation[1] = menu_navigation[0];
                menu_navigation[0] = Constants::PAUSE_MENU;
            }
        }
        else {
            main_theme_fade_current = Registry::run_time;
            main_theme.setVolume(interpolate_value(current_volume, 15, 1, main_theme_fade_current - main_theme_fade_start));
        }

        window.clear(Constants::SKY_BLUE);

        for (int i = 0; i < 15; i++) {
            clouds[i].compute();
            clouds[i].render(window);
        }

        ground.compute();
        ground.render(window);

        if (menu_navigation[0] == Constants::MAIN_MENU) {
            Registry::score = 0;
            next_component = main_menu.run_menu(window, player_input);
            if (next_component != Constants::MAIN_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::MAIN_MENU;
                main_menu.unload();
                level_one.reset_level(); // for when returning from pause menu
                level_two.reset_level(); // for when returning from pause menu
            }
        }
        else if (menu_navigation[0] == Constants::LEVEL_ONE) {
            next_component = level_one.run_menu(window, player_input);
            if (next_component != Constants::LEVEL_ONE) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::LEVEL_ONE;
                level_one.unload();
                if (next_component != Constants::PAUSE_MENU) {
                    level_one.reset_level();
                }
            }
        }
        else if (menu_navigation[0] == Constants::LEVEL_TWO) {
            next_component = level_two.run_menu(window, player_input);
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
            next_component = end_menu.run_menu(
                window,
                player_input);

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
                player_input);

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
            break;
        }
        else {
            cout << "Current level not recognised!!!";
            menu_navigation[0] = Constants::MAIN_MENU;
            menu_navigation[1] = Constants::MAIN_MENU;
        }

        window.display();
        player_input.update(window);
        Registry::run_time += 1 / 60.0;
    }

    return 0;
}