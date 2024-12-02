#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <vector>

#include "GameObjects.h"

using namespace std;

float pythagorean_distance(float x1, float y1, float x2, float y2);

float random_float(float minimum, float maximum);

int calculate_point_count(int radius, float shape_quality = 0.75);

string path_builder(string(&args)[4]);

void play_random_pop_sounds(int count);

void play_cannon_fire_sound();

void play_anvil_fail_sound();

sf::Color pick_ball_color();

bool spread_group_flags(vector<Ball>& game_balls);

bool spread_anchor_flags(vector<Ball>& game_balls);

int count_group_flags(vector<Ball>& game_balls);