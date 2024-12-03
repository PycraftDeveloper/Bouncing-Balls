#include "Registry.h"

namespace Registry {
    int window_size[2] = { 1280, 720 };  // Example size
    std::string game_end_state = "won";
    int ball_radius = window_size[1] / (14 * 2);
    float ball_tesselation_coefficient = sqrt(3) * ball_radius;
    int score = 0;
    float run_time = 0;
}
