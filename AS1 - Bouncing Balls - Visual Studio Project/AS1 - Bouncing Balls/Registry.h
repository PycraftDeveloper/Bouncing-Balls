#pragma once

#include <SFML/Audio.hpp>
#include <string>

using namespace std;

#ifndef REGISTRY_H
#define REGISTRY_H

namespace Registry {
	extern int window_size[2];
	extern string game_end_state;
	extern float BALL_RADIUS;
	extern sf::Music pop_sounds[15];
	extern sf::Music cannon_fire_sound;
}

#endif