/*
This program is used to store a collection of commonly used functions that themselves can't be categorized
into a specific object, or that are used by multiple objects. This provides code for the functions first outlined in
the 'UtilityFunctions.h' header file.
*/

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
        return end_value; // used to ensure the interpolation function doesn't start to extrapolate beyond
        // the target duration.
    }
    return start_value + ((end_value-start_value) / target_duration) * current_duration;
}

float pythagorean_distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

float random_float(float minimum, float maximum) {
    float random = static_cast<float>(rand()) / RAND_MAX; // this is used to generate a random
    // integer number between 0 and the maximum random value (which can be identified using the RAND_MAX)
    // constant. Then this random integer value is converted into a float between 0 and 1 (inclusive) by dividing
    // the value by the RAND_MAX constant. This allows for the generation of a random floating point value.
    return minimum + random * (maximum - minimum); // the previously generated floating point value is then converted
    // from the range [0, 1] to [minimum, maximum] using the following calculation, which was initially identified by
    // using the interpolation equation from earlier.
}

int calculate_point_count(int radius, float shape_quality) {
    radius = abs(radius); // The radius of the circle does not depend on positive and negative values. A negative value
    // would just mean that point on the circumference on the circle is flipped. Therefore only the magnitude of radius
    // is important in this calculation.
    if (radius < 1) {
        return 3; // if the radius is less than 1, it is 0. In this situation the resulting calculation is mathematically
        // undefined. Therefore the minimum value for the number of points, 3, is returned instead.
    }
    float angle = asin(1.0f / radius);
    int point_count = 1 + int((Constants::TAU / angle) * shape_quality); // this is the calculation to determine the number
    // of points needed to accurately represent a circle on-screen with given radius. The shape quality is considered here as
    // the window can only display a digital image, not an analogue one so the number of points can be reduced slightly to improve
    // rendering performance, without sacrificing on image quality.

    if (point_count < 3) {
        point_count = 3; // again here it is ensured that the smallst possible number of points returned is 3.
    }

    return point_count;
}

string path_builder(string(&args)[4]) {
    // The size of the args array being set to 4 represents the relative maximum depth through the game's directory tree
    // that a file can be located.
    string resultant_path = "";
    int length_of_input_array = sizeof(args) / sizeof(args[0]);
    for (int i = 0; i < length_of_input_array; i++) {
        if (args[i] == "") {
            break;
        }
        resultant_path += "\\" + args[i]; // this for loop is used to take a series of individual directories and files and
        // convert them into a single file path string which can be understood by SFML. It should be noted that the '\\' could be
        // changed or expanded upon to include support for other path separators, introducing additional cross compatibility.
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
    bool changed = false; // the changed flag is initially set to this value, as when a change is
    // made, the flag can be updated, which tells the caller if the function needs to be called again
    // in a similar approach to how the bubble sort algorithm works.
    for (int i = 0; i < game_balls.size() - 2; i++) {
        // the last two balls belong to the cannon and aren't ready to be fired yet.
        for (int j = 0; j < game_balls.size() - 2; j++) {
            // here, every ball in the level (aside from the balls belonging to the cannon) are checked against
            // every other ball (again not including the ones belonging to the cannon).
            if (game_balls[i].check_collision_with_flag_ball(game_balls[j])) {
                // check if ball in collision with one with group flag.
                game_balls[i].set_group_flag(true); // here the group flag is spread when the balls are in collision
                // and one of the two colliding balls already have the flag.
                changed = true;
            }
        }
    }
    return changed;
}

bool spread_anchor_flags(vector<Ball>& game_balls) {
    bool changed = false; // the changed flag is initially set to this value, as when a change is
    // made, the flag can be updated, which tells the caller if the function needs to be called again
    // in a similar approach to how the bubble sort algorithm works.
    for (int i = 0; i < game_balls.size() - 2; i++) {
        // the last two balls belong to the cannon and are't ready to be fired yet.
        for (int j = 0; j < game_balls.size() - 2; j++) {
            // here, every ball in the level (aside from the balls belonging to the cannon) are checked against
            // every other ball (again not including the ones belonging to the cannon).
            if (game_balls[i].check_collision_with_anchor_ball(game_balls[j])) {
                // check if ball in collision with one with anchor flag.
                game_balls[i].set_anchored(true); // when a ball is in collision with another ball that has
                // the anchor flag applied to it, then the flag must spread to the ball it is in collision with.
                changed = true;
            }
        }
    }
    return changed;
}

int count_group_flags(vector<Ball>& game_balls) {
    int count = 0;
    for (int i = 0; i < game_balls.size() - 2; i++) {
        // the game iterates over all the balls in the level, excluding the ones
        // that belong to the cannon.
        if (game_balls[i].get_group_flag()) {
            // only the balls that have the group flag count toward the total.
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
    string line; // this stores the current text data from the line read by the file
    string split_line[2]; // this is used to store the two components of the line,
    // each line consists of an identifier, and a value, these two strings are then stored
    // in this array: {identifier, value}.

    string path_components[4] = { "game_configuration",
            "save.txt" };

    ifstream game_save(path_builder(path_components)); // open the text file at the path specified, and prepare it
    // for read operations.

    bool seperator_found = false; // this flag is used to determine if the separator ":" has been found,
    // and this is used to differentiate the identifier from the value.

    if (game_save.is_open()) { // if the file was able to be opened (indicating a previous save is available to read).
        while (getline(game_save, line)) {
            // here, the file is read, line by line from the top of the file, and each line is read into the line variable.
            split_line[0] = "";
            split_line[1] = ""; // clear and initialize the two elements in the array from earlier.
            seperator_found = false; // clear any previously modified flags.
            for (int i = 0; i < line.size(); i++) {
                // here, the code iterates over each character in the string, to determine its form.
                if (seperator_found) {
                    split_line[1] += line[i];
                }
                if (line[i] == ':') {
                    seperator_found = true;
                }
                else if (seperator_found == false) {
                    split_line[0] += line[i];
                }
                // here the raw line content is split up into its two components, which can later be analysed.
                // effectively converting from human readable to machine readable.
            }
            
            // now the identifiers are checked to see if they correspond to the two conditions here, allowing the value to overwrite
            // registry values with the same name.
            if (split_line[0] == "Registry.play_music") {
                Registry::play_music = string_to_bool(split_line[1]);
            } 
            else if (split_line[0] == "Registry.play_sounds") {
                Registry::play_sounds = string_to_bool(split_line[1]);
            }
        }
        // once the file has been read, its good practise to close it.
        game_save.close();
        // because the file was successfully read, we don't want to overwrite its content yet, so we inform the game of the successfully
        // read operation.
        return true;
    }
    // if there is no previous file to open, then that means it needs to be created, and the boolean returned is used to
    // indicate first if a save was loaded, and subsequently if the configuration file need writing (using the default values
    // in the registry).
    return false;
}

void write_save_data() {
    string path_components[4] = { "game_configuration",
            "save.txt"};

    ofstream game_save(path_builder(path_components), ios::trunc);  // open the text file at the path specified, and prepare it
    // for write operations, ensuring the previous content is first cleared.

    if (game_save.is_open()) {
        // if the file was opened correctly, then the relevant data can be formatted and written (line by line) to the file.
        game_save << "Registry.play_music:" << bool_to_string(Registry::play_music) << endl;
        game_save << "Registry.play_sounds:" << bool_to_string(Registry::play_sounds) << endl;
        // then the file is closed afterwards, saving the changes made.
        game_save.close();
    }
    else {
        cout << "Unable to open file!!!" << endl;
    }
}