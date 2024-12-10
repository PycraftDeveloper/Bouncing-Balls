/*
This program stores all the globally accessed variables used throughout the Bouncing Balls program. This is
in principle somewhat similar to the constants, except that these values can be changed during runtime by any
reference to the name. It should be noted here that this is where the variables defined in the 'registry.h' file
are assigned their initial values.
*/
#include "Registry.h"
#include "Constants.h"

namespace Registry {
    int window_size[2] = { 1280, 720 }; // this sets the game size to a 16:9 aspect ratio.
    std::string game_end_state = Constants::WIN; // this sets the initial end menu to be in a 'win' state
    // however this is overridden by the game state in each level as needed.
    int ball_radius = window_size[1] / (14 * 2); // this sets the ball radius depending on the size of the window
    // in the Y axis.
    float ball_tesselation_coefficient = sqrt(3) * ball_radius; // this uses the equation 'tessellation = (3**0.5)*r' to calculate the
    // right amount to move each row of balls up by to ensure a proper grid is formed.
    int score = 0; // this sets the initial game score to 0, this is reset when the player is in the main menu as
    // it is not possible from the main menu to return to any previously played levels without first resetting them.
    float run_time = 0; // this sets the initial game run time to 0 seconds.
}
