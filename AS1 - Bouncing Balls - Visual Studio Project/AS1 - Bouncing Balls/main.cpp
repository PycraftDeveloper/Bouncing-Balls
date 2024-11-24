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

int main() // https://learn.microsoft.com/en-us/cpp/code-quality/c6262?view=msvc-170 17.412 KB of stack!!!
// ~ 2660 lines, PMMA is: 226511 lines or ~ 1482710 Bytes of stack or 1.48271 MB!!!
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
    bool game_won = false;
    bool window_full_screen = false;
    bool game_running = true;

    string next_component;

    // set-up game componentry
    PlayerInput player_input = PlayerInput();

    // set-up game levels
    MainMenu main_menu = MainMenu();
    LevelOne level_one = LevelOne(window);
    LevelTwo level_two = LevelTwo(window);
    GameEndMenu end_menu = GameEndMenu();
    PauseMenu pause_menu = PauseMenu();

    Cloud clouds[15];
    Ground ground;

    // Load pop sounds. Using music here means files aren't actually loaded
    // and instead streamed as needed, meaning this isn't actually that
    // inefficient!
    for (int i = 0; i < 15; i++) { // 16
        string file = to_string(i + 1) + ".wav";
        string path_components[50] = { "resources", "sounds", "pops", file };
        string file_path = path_builder(path_components);
        Registry::pop_sounds[i].openFromFile(file_path);
    }

    string cannon_fire_sound_path_components[50] = { "resources", "sounds", "cannon fire.wav" };
    Registry::cannon_fire_sound.openFromFile(path_builder(cannon_fire_sound_path_components));
    Registry::cannon_fire_sound.setVolume(30);

    sf::Music main_theme;
    string path_components[50] = { "resources", "music", "main theme [extended].ogg"};
    main_theme.openFromFile(path_builder(path_components));
    main_theme.setLoop(true);
    main_theme.setVolume(15);
    //main_theme.play();

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
                    } else if (menu_navigation[0] == Constants::LEVEL_ONE || menu_navigation[0] == Constants::LEVEL_TWO) {
                        menu_navigation[1] = menu_navigation[0];
                        menu_navigation[0] = Constants::PAUSE_MENU;
                        // note, this is flipped compared to above because exiting the pause menu
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

        window.clear(Constants::SKY_BLUE);

        for (int i = 0; i < 15; i++) {
            clouds[i].compute();
            clouds[i].render(window);
        }

        ground.compute();
        ground.render(window);

        if (menu_navigation[0] == Constants::MAIN_MENU) {
            next_component = main_menu.run_menu(window, player_input);
            if (next_component != Constants::MAIN_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::MAIN_MENU;
            }
        }
        else if (menu_navigation[0] == Constants::LEVEL_ONE) {
            next_component = level_one.run_menu(window, player_input);
            if (next_component != Constants::LEVEL_ONE) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::LEVEL_ONE;
                if (next_component != Constants::PAUSE_MENU) {
                    level_one.reset_level(window);
                }
            }
        }
        else if (menu_navigation[0] == Constants::LEVEL_TWO) {
            next_component = level_two.run_menu(window, player_input);
            if (next_component != Constants::LEVEL_TWO) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::LEVEL_TWO;
                if (next_component != Constants::PAUSE_MENU) {
                    level_two.reset_level(window);
                }
            }
        }
        else if (menu_navigation[0] == Constants::END_MENU) {
            next_component = end_menu.run_menu(
                window,
                player_input,
                game_won);

            if (next_component != Constants::END_MENU) {
                menu_navigation[0] = next_component;
                menu_navigation[1] = Constants::END_MENU;
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
    }

    return 0;
}