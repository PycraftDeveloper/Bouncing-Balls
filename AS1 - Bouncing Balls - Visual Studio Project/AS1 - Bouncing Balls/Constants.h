/*
This program stores all the constant values needed in the Bouncing Balls program. This is somewhat similar
in principle to the registry, except these value cannot be changed during the runtime of the program.
Unlike the registry, the names and values are declared together here, as there is no 'constants.cpp' file.
*/
#pragma once
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    // All the constants used throughout the program are contained within the 'Constants' namespace.
    // this has been done deliberately to 'bundle' them together, as to avoid accidentally attempting
    // to update them in code, which would result in an error as all the values have been declared
    // with the 'const' constraint.
    const string MAIN_MENU{ "main menu"};
    const string LEVEL_ONE{ "level one"};
    const string LEVEL_TWO{ "level two" };
    const string END_MENU{ "end menu" };
    const string PAUSE_MENU{ "pause menu" };
    // The 5 constants above are used to uniquely address each of the different available
    // menus in the game. The string data type has been used here, as it makes determining
    // which menu is being used much easier for debugging than for example an integer system.

    const string QUIT{ "quit" }; // this is somewhat similar to the constants defined above in
    // its use in the 'menu navigator' in main, except that instead of taking you to a menu, it 
    // instead tells the game loop to exit. 

    const string FONT_PLAY{ "play font" };
    const string FONT_REGULAR{ "play regular" };
    // these two constants represent the two font files available for rendering in the game. Because the
    // fonts used by the game is limited to two, instead of specifying the file path, this can be used and
    // the text renderer will know what font this corresponds to.

    const string WIN{ "won" };
    const string LOST{ "lost" };
    // These represent the possible states of the game once it has been completed. This is used to control
    // how the end menu renders.

    const string LEFT{ "left" };
    const string RIGHT{ "right" };
    // these two constants represent the sides of the window, which in turn can be used for positioning
    // of content on-screen.

    constexpr int BALL_SPEED{ 20 }; // This represents how many pixels the ball should move when fired from the
    // cannon every frame.
    constexpr float MASS_FALL_SPEED{ 3.6/60.0 }; // This represents how quickly the mass should move down the screen.
    // because the frame rate is locked to 60, and rarely drops below this, this value can be considered as constant.
    // the '3.6' then represents how far to move the mass in pixels every second, and roughly corresponds to the fall
    // speed of the mass in the game 'Bouncing Balls' NOT the 'Puzzle Bobble' game. This in turn means that similar to
    // the 'Bouncing Balls' game each level lasts approximately 1 minute and 30 seconds at the set window size.

    constexpr float PI{ 3.14159 };
    constexpr float TAU{ 6.28319 }; // 2 * PI
    constexpr float RADIANS_TO_DEGREES_CONVERSION_CONSTANT{ 57.29578 }; // 180 / PI
    constexpr float DEGREES_TO_RADIANS_CONVERSION_CONSTANT{ 0.01745 }; // PI / 180
    // these represent mathematical constants which are primarily used in calculating the number of points in each circle
    // for smoother rendering, and for calculating the angle for the cannon and the balls it fires.

    const sf::Color SKY_BLUE{ sf::Color(192, 211, 234) }; // this represents the colour of the background in the game, and has
    // specifically been picked as blue as it is well regarded as a relaxing colour:
    // https://www.researchgate.net/profile/Lesley-Lubos/publication/314578015_The_Role_of_Colors_in_Stress_Reduction/links/5baf102e92851ca9ed2e6128/The-Role-of-Colors-in-Stress-Reduction.pdf
    // accessed 10/12/2024 @ 15:45 AND 20/11/2024 @ 12:41


    const float ACCELERATION_DUE_TO_GRAVITY = 9.806; // this is a physics constant and represents an approximation for the acceleration
    // experienced by an object in free-fall in earth's gravitational field at a distance of 6.38 million meters from its centre in units: m/s**2.
    // https://physics.nist.gov/cgi-bin/cuu/Value?gn accessed 10/12/2024 @ 15:51.
}

#endif