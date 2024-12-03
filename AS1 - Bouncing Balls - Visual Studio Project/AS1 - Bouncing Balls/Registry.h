#pragma once
#pragma warning( disable : 4244 )

#include <SFML/Audio.hpp>
#include <string>

using namespace std;

#ifndef REGISTRY_H
#define REGISTRY_H

namespace Registry {
	extern int window_size[2];
	extern string game_end_state;
	extern int ball_radius;
	extern float ball_tesselation_coefficient;
	extern int score;
	extern float run_time;
}

#endif