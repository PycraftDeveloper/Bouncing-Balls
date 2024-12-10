/*
This program is used to store a collection of commonly used functions that themselves can't be categorized
into a specific object, or that are used by multiple objects. This is the header file and the actual code content
is defined in 'UtilityFunctions.cpp'.
*/
#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <vector>

#include "GameObjects.h"

using namespace std;

float interpolate_value(float start_value, float end_value, float target_duration, float current_duration);
// this is a form of linear interpolation algorithm, except this will always return 'end_value' when 'current_duration'
// is greater than 'target_duration'.

float pythagorean_distance(float x1, float y1, float x2, float y2); // this function uses Pythagorean theorem to determine the
// distance between two points. The equation is defined as:
// a = ((x2-x1)**2 + (y2-y1)**2)**0.5
// where a is the value returned.

float random_float(float minimum, float maximum); // this is used to generate a random floating point number between two
// values. 

int calculate_point_count(int radius, float shape_quality = 0.75); // this is used to generate a value for the number of points
// to use in the creation of a circle, based on the radius of said circle. This is done to improve the efficiency of the program
// as fewer points are needed to represent circles of smaller radius than the SFML default number of points. This is done to improve
// the games graphical fidelity. The second value in this function represents a quality, as it often isn't necessary to include
// a perfect circle, so a middle ground must be calculated between the default SFML value and the maximum number of points needed.

string path_builder(string(&args)[4]); // this is used to take an array of 4 components, each representing a file or folder
// in the games directory. It then constructs these elements into a single file path as a string, relative to the project location
// so no adjustments are necessary when running the visual studio project from another computer. Furthermore, this could be expanded to
// include different path separators for different operating systems, which would make the game cross platform.

sf::Color pick_ball_color(); // this is used to randomly select a ball colour and it can be either: 'Red', 'Yellow' or 'Blue'.

bool spread_group_flags(vector<Ball>& game_balls); // this forms part of the flag spreading algorithm. Here the 'group' flag is being
// spread, which is a property of each ball and used to determine if the ball fired forms a group of 3 or more balls. It should be
// noted that this function returns a boolean value, true if a flag has spread, and false if it hasn't which is used to determine when
// there have been no changes to the flag arrangement in the game_balls array

bool spread_anchor_flags(vector<Ball>& game_balls); // this also forms part of the flag spreading algorithm, except this time it is
// used to determine if a ball in the game_balls array has a connection to other balls which are anchored to the mass at the top of the level.
// this is then used to determine if a ball should fall as it is left suspended. Similar to the previous function, it too returns a boolean
// value depending on if a flag has spread in the game_balls array.

int count_group_flags(vector<Ball>& game_balls); // this is used to count all the balls in the game_balls array that have the group
// flag, and this is returned back to the caller. This is important as it is used to directly determine when a group of 3 or more balls has
// been identified.