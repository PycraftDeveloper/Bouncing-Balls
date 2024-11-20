#include <SFML/Audio.hpp>
#include <string>

using namespace std;

#ifndef REGISTRY_H
#define REGISTRY_H

namespace Registry {
	int window_size[2] = { 12890, 720 };
	string game_end_state = "won";
	float BALL_RADIUS = window_size[1] / (14.0 * 2);
	sf::Music pop_sounds[15];
	sf::Music cannon_fire_sound;
}

#endif