#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Constants.h"
#include "Registry.h"
#include "GameObjects.h"

using namespace std;

float pythagorean_distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

float random_float(float minimum, float maximum) {
    float random = static_cast<float>(rand()) / RAND_MAX;
    return minimum + random * (maximum - minimum);
}

int calculate_point_count(int radius, float shape_quality) {
    if (radius < 1) {
        return 3;
    }
    float angle = asin(1.0f / radius);
    int point_count = 1 + int((Constants::TAU / angle) * shape_quality);

    if (point_count < 3) {
        point_count = 3;
    }

    return point_count;
}

string path_builder(string(&args)[4]) { // 4 matches the furthest point into the resources directory

    string resultant_path = "";
    int length_of_input_array = sizeof(args) / sizeof(args[0]);
    for (int i = 0; i < length_of_input_array; i++) {
        if (args[i] == "") {
            break;
        }
        resultant_path += "\\" + args[i];
    }
    return resultant_path.substr(1, resultant_path.size());
}

void play_random_pop_sounds(int count) {
    for (int i = 0; i < count; i++) {
        int sound_index = rand() % 15;
        Registry::pop_sounds[sound_index].play();
    }
    // it is possible to do this serially. By creating custom music objects 
    // with a play function that mimics the music.play function and then 
    // updating these objects every frame with custom delays.
    // Note: doing this would require that the audio isn't already in its 
    // delay state which could easily be added in. This limits max number 
    // of sounds to 15, as that's how many unique files we have,
    // however clever sound design should be able to mask this limitation.
}

void play_cannon_fire_sound() {
    Registry::cannon_fire_sound.stop();
    Registry::cannon_fire_sound.play();
}

void play_anvil_fail_sound() {
    Registry::anvil_fail_sound.stop();
    Registry::anvil_fail_sound.play();
}

sf::Color pick_ball_color() {
    int randomly_chosen_color = rand() % 3;
    if (randomly_chosen_color == 0) {
        return sf::Color::Blue;
    }
    else if (randomly_chosen_color == 1) {
        return sf::Color::Yellow;
    }
    else {
        return sf::Color::Red;
    }
}

bool spread_group_flags(vector<Ball>& game_balls) {
    bool changed = false;
    for (int i = 0; i < game_balls.size() - 2; i++) {
        // the last two balls belong to the cannon and are't ready to be fired yet.
        for (int j = 0; j < game_balls.size() - 2; j++) {
            if (game_balls[i].check_collision_with_flag_ball(game_balls[j])) {
                // check if ball in collision with one with group flag.
                game_balls[i].group_flag = true;
                changed = true;
            }
        }
    }
    return changed;
}

bool spread_anchor_flags(vector<Ball>& game_balls) {
    bool changed = false;
    for (int i = 0; i < game_balls.size() - 2; i++) {
        // the last two balls belong to the cannon and are't ready to be fired yet.
        for (int j = 0; j < game_balls.size() - 2; j++) {
            if (game_balls[i].check_collision_with_anchor_ball(game_balls[j])) {
                // check if ball in collision with one with group flag.
                game_balls[i].anchored_flag = true;
                changed = true;
            }
        }
    }
    return changed;
}

int count_group_flags(vector<Ball>& game_balls) {
    int count = 0;
    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].group_flag) {
            count++;
        }
    }
    return count;
}