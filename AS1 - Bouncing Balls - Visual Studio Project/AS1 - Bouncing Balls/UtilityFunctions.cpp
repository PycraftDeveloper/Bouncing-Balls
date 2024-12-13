#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

#include "Constants.h"
#include "Registry.h"
#include "GameObjects.h"

using namespace std;

float interpolate_value(float start_value, float end_value, float target_duration, float current_duration) {
    if (current_duration > target_duration) {
        return end_value;
    }
    return start_value + ((end_value-start_value) / target_duration) * current_duration;
}

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
                game_balls[i].set_group_flag(true);
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
                game_balls[i].set_anchored(true);
                changed = true;
            }
        }
    }
    return changed;
}

int count_group_flags(vector<Ball>& game_balls) {
    int count = 0;
    for (int i = 0; i < game_balls.size() - 2; i++) {
        if (game_balls[i].get_group_flag()) {
            count++;
        }
    }
    return count;
}

string bool_to_string(bool value) {
    if (value) {
        return Constants::TRUE;
    }
    return Constants::FALSE;
}

bool string_to_bool(string text) {
    if (text == Constants::TRUE) {
        return true;
    }
    return false;
}

bool read_save_data() {
    string line;
    string split_line[2];

    string path_components[4] = { "game_configuration",
            "save.txt" };

    ifstream game_save(path_builder(path_components));

    int index = 0;
    bool seperator_found = false;

    if (game_save.is_open()) {
        while (getline(game_save, line)) {
            split_line[0] = "";
            split_line[1] = "";
            seperator_found = false;
            for (int i = 0; i < line.size(); i++) {
                if (seperator_found) {
                    split_line[1] += line[i];
                }
                if (line[i] == ':') {
                    seperator_found = true;
                }
                else if (seperator_found == false) {
                    split_line[0] += line[i];
                }
            }
            
            index++;
            if (split_line[0] == "Registry.play_music") {
                Registry::play_music = string_to_bool(split_line[1]);
            } 
            else if (split_line[0] == "Registry.play_sounds") {
                Registry::play_sounds = string_to_bool(split_line[1]);
            }
        }
        game_save.close();
        return true;
    }
    return false;
}

void write_save_data() {
    string path_components[4] = { "game_configuration",
            "save.txt"};

    ofstream game_save(path_builder(path_components), ios::trunc);

    if (game_save.is_open()) {
        game_save << "Registry.play_music:" << bool_to_string(Registry::play_music) << endl;
        game_save << "Registry.play_sounds:" << bool_to_string(Registry::play_sounds) << endl;
        game_save.close();
    }
    else {
        cout << "Unable to open file!!!" << endl;
    }
}