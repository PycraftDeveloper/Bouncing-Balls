#include "Registry.h"

namespace Registry {
    int window_size[2] = { 1280, 720 };  // Example size
    std::string game_end_state = "won";
    float BALL_RADIUS = window_size[1] / (14.0f * 2);
    sf::Music pop_sounds[15];
    sf::Music cannon_fire_sound;
}
