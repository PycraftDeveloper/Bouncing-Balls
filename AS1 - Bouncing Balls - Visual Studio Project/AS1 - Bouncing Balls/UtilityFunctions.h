#pragma once

#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

float pythagorean_distance(float x1, float y1, float x2, float y2);

float random_float(float minimum, float maximum);

int calculate_point_count(int radius, float shape_quality = 0.75);

string path_builder(string(&args)[50]);

void play_random_pop_sounds(int count);

void play_cannon_fire_sound();

sf::Color pick_ball_color();