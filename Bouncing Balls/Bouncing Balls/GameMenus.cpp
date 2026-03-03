/*
This is one of the most important programs in the bouncing balls code. Here each game menu is defined, from the main menu to the
game levels. Furthermore, this program also dictates how each level connected to each other - but not how they get there. It should
also be noted that a lot of the functionality behind this menu is defined in the other programs and the game mechanics are largely
defined within the game objects. This is the program file for the structure outlined in the header file: 'GameMenus.h'
*/
#pragma warning( disable : 4244 )

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "GameMenus.h"
#include "UtilityObjects.h"
#include "GameObjects.h"
#include "Constants.h"
#include "Registry.h"
#include "UtilityFunctions.h"

using namespace std;

MainMenu::MainMenu() {
}

string MainMenu::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
	// this runs once every frame
	// top to bottom rendering here where layering doesn't matter
	// render game title
	bool play_button_result, quit_button_result;

	instructions.compute();
	left_dragon.compute(Constants::LEFT);
	right_dragon.compute(Constants::RIGHT);
	play_button_result = play_button.compute(player_input);
	quit_button_result = quit_button.compute(player_input);
	media_controls.compute(player_input);
	// All the elements in the menu are computed here

	title_text.set_position(window, -1, 0);

	instructions.render(window);
	left_dragon.render(window);
	right_dragon.render(window);
	title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);
	play_button.render(window, -1, -1, "Play!");
	quit_button.render(window, -1, -101, "quit");
	media_controls.render(window, -1, -201);
	// All the elements in the menu are rendered here - on top of the background content rendered in the main program.

	// identify what menu to transition to next
	if (quit_button_result) {
		return Constants::QUIT;
	}
	else if (play_button_result) {
		return Constants::LEVEL_ONE;
	}
	// by default return to this menu on next run
	return Constants::MAIN_MENU;
}

void MainMenu::unload() {
	left_dragon.unload();
	right_dragon.unload();
	instructions.unload();
	play_button.unload();
	quit_button.unload();
	title_text.unload();
	media_controls.unload();
	// This will pass unload commands to all the elements that loaded textures from files, and causes them to free up memory to save space.
}

LevelTemplate::LevelTemplate() {
	string cannon_fire_sound_path_components[4] = { "resources", "sounds", "cannon fire.ogg" };
	string anvil_fail_sound_path_components[4] = { "resources", "sounds", "anvil.ogg" };
	// Arrays are constructed that store the file components representing the locations of sounds this class and the menus it is inhrited by use.

	for (int i = 0; i < 15; i++) { // 16
		string file = to_string(i + 1) + ".ogg";
		string pop_sound_path_components[4] = { "resources", "sounds", "pops", file };
		string file_path = path_builder(pop_sound_path_components);
		pop_sounds[i].openFromFile(file_path);
	} // Each of the 15 different 'pop' sounds are also loaded from similar paths here.

	cannon_fire_sound.openFromFile(path_builder(cannon_fire_sound_path_components));
	cannon_fire_sound.setVolume(30);

	anvil_fail_sound.openFromFile(path_builder(anvil_fail_sound_path_components));
	anvil_fail_sound.setVolume(40);
	// The arrays constructed earlier are then used to load audio files and their volumes are adjusted to balance the different sounds in the game
	// it should be noted that because the sounds are all streamed, they don't get loaded and unloaded by the program as their size in memory is much smaller
	// then other content loaded from disk.
}

void LevelTemplate::create_ball_grid(vector<Ball>& game_balls, string current_level) {
	int columns = 14;
	int rows = 6;

	if (current_level == Constants::LEVEL_TWO) {
		rows = 8;
	}

	int x_pos = Registry::ball_radius;
	int y_pos = Registry::ball_radius + mass_object.get_game_ceiling();
	// This sets the initial x and y positions for the ball to the top-left corner of the window (with a bit of padding to ensure the whole ball is
	// visible immediately).
	for (int row = 0; row < rows; row++) {
		if (row % 2 == 0) {
			columns = 14;
			x_pos = Registry::ball_radius;
		}
		else {
			columns = 13;
			x_pos = Registry::ball_radius * 2;
		}
		// The rows are set to interlock, with rows varying between 13 and 14 different balls. This is defined here and the x_position is offset to ensure this.
		for (int column = 0; column < columns; column++) {
			Ball game_ball = Ball(x_pos + mass_object.get_x_position(), y_pos);
			game_balls.emplace_back(game_ball);
			x_pos += Registry::ball_radius * 2;
		} // Each new ball is added to the end of the game_balls array, with the cannon balls not yet existing, there is no need to re-arrange the array.
		// The x_position is then increased to ensure no balls overlap in the x axis.
		y_pos += Registry::ball_tesselation_coefficient; // Then, each row of balls is moved slightly to ensure that they appear in a tighter pattern, akin somewhat
		// to a tessellation. This ensures each row is on contact with the rows above and below, ensuring the balls remain fixed to the top of the game area.
	}
}

string LevelTemplate::run_menu(sf::RenderWindow& window, PlayerInput& player_input, string current_menu) {
	// this runs once every frame
	float angle = 0;
	bool ball_in_motion = false;
	vector<int> garbage_ball_elements = {};
	int index = 0;
	int random_pop_sound_index, x_pos, y_pos;

	instructions.compute();
	left_dragon.compute(Constants::LEFT);
	right_dragon.compute(Constants::RIGHT);

	if (level_over == false) {
		cannon_object.compute(window, player_input, game_balls);
	} // The cannon object should in effect be de-activated when the level is over, otherwise the player could continue to interact with the level when they aren't
	// supposed to, causing bugs.

	// Handle all the compute first, then the rendering later. The compute can happen in any order, but the rendering must take into account what needs to be on
	// top. The mass object is a great example of this, being computed and rendered much later, because it needs to appear on top, and other compute logic must come first.

	instructions.render(window);
	left_dragon.render(window);
	right_dragon.render(window);

	score_text.set_position(window, 0, 0);
	score_text.render(window, "Score: " + to_string(Registry::score), 30, sf::Color::Black);

	if (level_over == false) {
		// The code here will only run when the level isn't over.
		cannon_object.render(window); // When the game is over, the cannon isn't rendered, because the mass either obscures it, or its no longer needed.
		for (auto& game_ball : game_balls) {
			if (index == game_balls.size() - 2) {
				angle = (cannon_object.get_rotation() - 90) * Constants::DEGREES_TO_RADIANS_CONVERSION_CONSTANT;

				x_pos = -sin(angle) * Registry::ball_radius / 2;
				y_pos = -cos(angle) * Registry::ball_radius / 2;
				game_ball.set_position(cannon_object.get_x_position() + x_pos, cannon_object.get_y_position() - y_pos);
			} // This is used to correctly position the balls owned by the 'cannon' so that they appear in the other they are fired
			// and reflect the changes in cannon position. This has no effect on the cannon firing the balls, which is handled elsewhere.

			game_ball.compute(mass_object);
			game_ball.render(window);
			if (game_ball.get_popped()) {
				garbage_ball_elements.emplace_back(index);
			} // Each element in the ball array is computed and rendered, then any balls that have been popped and removed from the scene are added to an array
			// that keeps track of this, then after the loop completes, these balls are removed from memory as they aren't needed anymore.

			else if (game_ball.get_x_velocity() == 0 && game_ball.get_y_velocity() == 0 && index < game_balls.size() - 2) {
				for (auto& collision_game_ball : game_balls) {
					game_ball.collision(collision_game_ball, game_balls);
				}
			} // This checks for collision between two balls not 'owned' by the cannon, where one of the balls in collision is stationary.
			index++; // keeps track of which ball is being computed in this iteration of the game loop. This also is used to keep track of the
			// indexes of the balls to be removed.
		}

		game_balls[game_balls.size() - 1].render(window); // The last ball in the array, the one next to fire is rendered again at the end, ensuring that it
		// appears above all others in the scene, and shows on top of other balls to fire in the cannon.

		sort(garbage_ball_elements.rbegin(), garbage_ball_elements.rend()); // The array of garbage ball indexes needs to be sorted because when the dynamic
		// array removes a ball, the indexes for the others change, so the balls need to be removed largest index to smallest to prevent this.

		for (int& garbage_ball : garbage_ball_elements) {
			game_balls.erase(game_balls.begin() + garbage_ball);
			Registry::score += 10; // each ball popped is worth 10 points.
		} // the balls no longer needed are removed from the scene, freeing up RAM.

		anchor_balls_to_mass(); // Now all the remaining balls in the scene are checked to ensure they are in some way connected to the mass at the top, otherwise they
		// will be flagged as needing to fall out the bottom of the window.

		index = 0;
		for (auto& game_ball : game_balls) {
			if (game_ball.get_x_velocity() != 0 || game_ball.get_y_velocity() != 0) {
				ball_in_motion = true;
			} // here a check is being made to ensure that if there is a ball that has been fired by the cannon previously, still moving around the level
			// the cannon cannot fire another until this ball has reached its target.
			if (game_ball.get_x_velocity() == 0 && game_ball.get_y_velocity() == 0 && index < game_balls.size() - 2) {
				if (game_ball.get_y_position() + Registry::ball_radius >= cannon_object.get_y_position() - cannon_object.get_height() / 2) {
					if (game_ball.get_is_ball_to_fall() == false) {
						game_lost = true;
						level_over = true;
					}
				}
			} // here a check is being made to see if any balls that form the grid of balls (and therefore not falling or a part of the cannon) is intersecting with
			// the y axis for the top of the cannon. This is used to see if the playr has won or lost the game.

			if (game_ball.pop_sound_needs_playing() && Registry::play_sounds) {
				random_pop_sound_index = rand() % 15;
				pop_sounds[random_pop_sound_index].play();
			} // If any of the balls need their pop sound effct playing, this is handled here after being popped earlier in the method.

			index++;
		}

		if (mass_object.get_y_position() >= cannon_object.get_y_position() - cannon_object.get_height() / 2) {
			game_lost = true;
			level_over = true;
		} // If the balls don't detect as causing the game to end, then the mass intersecting with the cannon certainly will, ensuring the game has a definitive end
		// although this should never end up running.
	}
	mass_object.render(window);
	mass_object.compute();
	// The mass is computed and rendered much later, ensuring it appears on top.

	if (level_over == false) {
		if (player_input.get_player_button_input() && ball_in_motion == false) {
			handle_fire_cannon_event(angle);
		} // if the level isn't yet over, the player is allowed to fire the cannon, provided that all previously fired balls have met their target
		// and the player makes to fire the cannon.
	}

	if (game_balls.size() == 2) {
		level_over = true;
	} // this checks to see if all the balls in the level (ignoring the balls 'owned' by the cannon) have been popped, thus the game is over.

	if (level_end_time == -1) {
		if (game_lost) {
			// If the game is lost, the mass moves to the bottom of the window, plays a sound and the game briefly pauses so the user can see that they failed
			// before going to the end menu. At this point various states in the game change, ensuring the game appears paused.
			level_end_time = Registry::run_time;
			mass_object.set_is_falling(false);
			mass_object.set_y_position(Registry::window_size[1] - mass_object.get_height());

			if (Registry::play_sounds) {
				anvil_fail_sound.play();
				for (int i = 0; i < 15; i++) {
					// play 15 random pop sounds (to indicate the mass squished all the balls)
					random_pop_sound_index = rand() % 15;
					pop_sounds[random_pop_sound_index].play();
				}
			}
		}
		else if (level_over) {
			// If the level is over, and the player hasn't lost, then the game pauses and then sets the mass to stop falling, as seen in the code above
			// except this time without the other indicators that they failed, because this time they won the level.
			mass_object.set_is_falling(false);
			level_end_time = Registry::run_time;
		}
	}

	if (level_over && game_lost == false) {
		// If the level is over, and the player has won, this is stated in the level, before the game moves on, either to the next level or the end menu.
		level_complete_text.set_position(window, -1, -1);
		level_complete_text.render(window, "Level Complete!", 50, sf::Color(60, 176, 67), true);
	}

	// Here, various states are used to determine which menu the game should transition to next, based on the state of the level after this frame has been calculated.
	if (Registry::run_time - level_end_time > 3) {
		// The game will pause for 3 seconds after a level has been finished, to show if the user won/lost.
		if (game_lost) {
			Registry::game_end_state = Constants::LOST;
			return Constants::END_MENU;
		}
		else if (level_over) {
			if (current_menu == Constants::LEVEL_ONE) {
				return Constants::LEVEL_TWO;
			}
			else if (current_menu == Constants::LEVEL_TWO) {
				Registry::game_end_state = Constants::WIN;
				return Constants::END_MENU;
			}
		}
	}

	return current_menu;
}

void LevelTemplate::anchor_balls_to_mass() {
	for (int i = 0; i < game_balls.size() - 2; i++) {
		if (game_balls[i].get_y_position() - Registry::ball_radius <= mass_object.get_game_ceiling() + 10) {
			game_balls[i].set_anchored(true);
		}
	} // This first part iterates over each ball in the array - ignoring the ones 'owned' by the cannon, and checks to se if they are immediately touching the mass
	// which acts as the game ceiling. If they are, then they are flagged.

	bool changed = true;
	while (changed) {
		changed = spread_anchor_flags(game_balls);
		// Then, until there are no more balls that can accept the flag, the flag is spread across the other game balls in direct contact with a ball with the flag.
	}

	for (int i = 0; i < game_balls.size() - 2; i++) {
		if (game_balls[i].get_anchored() == false && game_balls[i].get_x_velocity() == 0 && game_balls[i].get_y_velocity() == 0 && game_balls[i].get_is_ball_to_fall() == false) {
			game_balls[i].set_ball_to_fall(true);
		}
	} // Then any balls that don't have the flag, and naturally aren't currently being fired by the cannon, then the ball is set to fall. If the ball is being set to fall
	// for the first time, then it also adds 50 points to the score - once per ball.

	for (int i = 0; i < game_balls.size() - 2; i++) {
		game_balls[i].set_anchored(false);
	} // then the flags are reset, so the same process can occur again, in the next frame.
}

void LevelTemplate::handle_fire_cannon_event(float angle) {
	if (Registry::play_sounds) {
		cannon_fire_sound.play();
	}

	swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]);
	// First, the last two balls in the array are swapped. These are both owned by the cannon, and the aim here is to get the one being fired as close
	// as possible to the other balls in the array, not owned by the cannon.

	float x_velocity = sin(angle) * Constants::BALL_SPEED;
	float y_velocity = -cos(angle) * Constants::BALL_SPEED;

	game_balls[game_balls.size() - 2].set_velocity(x_velocity, y_velocity);
	// The ball is then assigned a velocity based on the angle of the cannon.

	cannon_object.load_cannon_with_ball(game_balls); // Then a new ball is added to the end of the array.
	swap(game_balls[game_balls.size() - 1], game_balls[game_balls.size() - 2]); // The oldest ball in the array is moved to the front, as its the next one
	// to fire, so the newly added ball becomes second in the queue. This is done so the balls are rendered and computed properly in the 'run_menu' method above.
	// Here is a breakdown of the way this works:
	// 0, 1, 2, 3, 4 | 5, 6
	// these represent the indexes for the balls, the last two separate.
	// the separator is moved to the right:
	// 0, 1, 2, 3, 4, 5 | 6
	// the new ball is added:
	// 0, 1, 2, 3, 4, 5 | 6, 7
	// The last two balls are swapped:
	// 0, 1, 2, 3, 4, 5 | 7, 6
	// Then the process repeats.
}

void LevelOne::init() {
	mass_object.compute(); // The mass object is computed so the game ceiling can be calculated for the game grid arrangement.

	create_ball_grid(game_balls, Constants::LEVEL_ONE); // The ball arrangement is created, then two further balls are added for the cannon. This is important
	// as the cannon's balls are always the last two.

	cannon_object.load_cannon_with_ball(game_balls);
	cannon_object.load_cannon_with_ball(game_balls);
}

void LevelOne::reset_level() {
	game_lost = false;
	level_over = false;
	level_end_time = -1;
	// This ensures the game can be replayed, with all 3 variables being reset to their initial values.

	mass_object.reset(); // puts the mass back to the top and allows it to fall

	game_balls.clear(); // removes all the existing balls from the scene (including the cannon balls)

	// Then basically re-initializes the class again
	mass_object.compute();

	create_ball_grid(game_balls, Constants::LEVEL_ONE);

	cannon_object.load_cannon_with_ball(game_balls);
	cannon_object.load_cannon_with_ball(game_balls);
}

void LevelTemplate::unload() {
	cannon_object.unload();
	mass_object.unload();
	left_dragon.unload();
	right_dragon.unload();
	instructions.unload();
	score_text.unload();
	level_complete_text.unload();
	// Passes an unload instruction through to all the game objects that loaded a texture from disk into RAM, so this space can be freed.
}

void LevelTwo::init() {
	// Here, the mass (and first few rows of balls) start above the screen and move down, ensuring that the user doesn't know how many balls
	// they will need to pop when the game starts.
	int ball_diameter = 2 * Registry::ball_radius;

	mass_object.set_vertical_offset(-2 * ball_diameter);

	// Everything below this is identical to the set-up for level one, just with a few extra rows of balls.
	mass_object.compute();

	create_ball_grid(game_balls, Constants::LEVEL_TWO);

	cannon_object.load_cannon_with_ball(game_balls);
	cannon_object.load_cannon_with_ball(game_balls);
}

void LevelTwo::reset_level() {
	int ball_diameter = 2 * Registry::ball_radius;

	game_lost = false;
	level_over = false;
	level_end_time = -1;

	mass_object.reset();

	game_balls.clear();

	mass_object.set_vertical_offset(-2 * ball_diameter);
	mass_object.compute();

	create_ball_grid(game_balls, Constants::LEVEL_TWO);

	cannon_object.load_cannon_with_ball(game_balls);
	cannon_object.load_cannon_with_ball(game_balls);
	// This is identical to the reset for level one, except this time with the initialization for level two, not level one.
}

GameEndMenu::GameEndMenu() {
}

string GameEndMenu::run_menu(sf::RenderWindow& window, PlayerInput& player_input) {
	// this runs once every frame
	// render game title
	bool play_again_button_result, quit_button_result;

	left_dragon.compute(Constants::LEFT);
	right_dragon.compute(Constants::RIGHT);
	play_again_button_result = play_again_button.compute(player_input);
	quit_button_result = quit_button.compute(player_input);
	media_controls.compute(player_input);
	// compute each of the on-screen elements, and determine if the buttons in the level have been clicked - storing the result.

	title_text.set_position(window, -1, 0); // here the title is rendered centred in the x axis with the -1.
	game_win_state.set_position(window, -1, 150); // render game win state (win/loose)
	game_score_result.set_position(window, -1, 200); // render the game score.

	string game_win_state_text_content;
	sf::Color game_win_state_text_color;
	if (Registry::game_end_state == Constants::WIN) {
		game_win_state_text_content = "You WIN!!!";
		game_win_state_text_color = sf::Color(60, 176, 67); // darker shade of green
	}
	else {
		game_win_state_text_content = "You Lost!!!";
		game_win_state_text_color = sf::Color::Red;
	}
	// This will change the colour of the end result based on the way the game ended. If the game was successfully completed, then the resulting text appears green,
	// red for failure.

	left_dragon.render(window);
	right_dragon.render(window);
	title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);
	game_win_state.render(window, game_win_state_text_content, 50, game_win_state_text_color, true);
	game_score_result.render(window, "Your Score: " + to_string(Registry::score), 50, sf::Color::Black);
	play_again_button.render(window, -1, -1, "Play Again!");
	quit_button.render(window, -1, -101, "Quit");
	media_controls.render(window, -1, -201);
	// render all the graphical content to the screen.

	// identify what menu to transition to next, based on the button the user clicks, for which the result was identified earlier.
	if (quit_button_result) {
		return Constants::QUIT;
	}
	else if (play_again_button_result) {
		return Constants::MAIN_MENU;
	}

	// by default return to this menu on next run
	return Constants::END_MENU;
}

void GameEndMenu::unload() {
	left_dragon.unload();
	right_dragon.unload();
	title_text.unload();
	game_win_state.unload();
	game_score_result.unload();
	play_again_button.unload();
	quit_button.unload();
	media_controls.unload();
	// Unloads all graphical content to save memory.
}

PauseMenu::PauseMenu() {
}

string PauseMenu::run_menu(
	sf::RenderWindow& window,
	string* menu_navigation,
	PlayerInput& player_input) {
	// this runs once every frame
	bool resume_button_result, main_menu_button_result, quit_button_result;

	instructions.compute();
	resume_button_result = resume_button.compute(player_input);
	main_menu_button_result = main_menu_button.compute(player_input);
	quit_button_result = quit_button.compute(player_input);
	media_controls.compute(player_input);
	// This section computes all the different on-screen elements, and in doing so also determines if the buttons in the menu have been clicked.

	title_text.set_position(window, -1, 0); // remember, -1 is centred in that axis.

	instructions.render(window);
	title_text.render(window, "Bouncing Balls", 100, sf::Color::Black, true);
	resume_button.render(window, -1, -1, "Resume Game");
	main_menu_button.render(window, -1, -101, "Main Menu");
	quit_button.render(window, -1, -201, "Quit");
	media_controls.render(window, -1, -301);
	// Render all the graphical content to the window.

	// determine which menu to transition to next, based on the results of the buttons - or stay on the current menu if the user hasn't interacted with any of the buttons.
	if (quit_button_result) {
		return Constants::QUIT;
	}

	if (main_menu_button_result) {
		return Constants::MAIN_MENU;
	}

	if (resume_button_result) {
		return menu_navigation[1];
		// go to previous level
	}
	return Constants::PAUSE_MENU; // stay on current window
}

void PauseMenu::unload() {
	instructions.unload();
	title_text.unload();
	resume_button.unload();
	quit_button.unload();
	main_menu_button.unload();
	media_controls.unload();
	// unload content loaded from disk into memory to save space. This content will only be re-loaded into memory if needed.
}