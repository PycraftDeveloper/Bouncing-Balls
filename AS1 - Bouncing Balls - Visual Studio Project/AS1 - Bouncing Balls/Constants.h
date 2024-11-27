#pragma once
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    const string MAIN_MENU{ "main menu"};
    const string LEVEL_ONE{ "level one"};
    const string LEVEL_TWO{ "level two" };
    const string END_MENU{ "end menu" };
    const string PAUSE_MENU{ "pause menu" };

    const string QUIT{ "quit" };

    const string FONT_PLAY{ "play font" };
    const string FONT_REGULAR{ "play regular" };

    const string WIN{ "won" };
    const string LOST{ "lost" };

    const string LEFT{ "left" };
    const string RIGHT{ "right" };

    constexpr int BALL_SPEED{ 20 };
    constexpr float MASS_FALL_SPEED{ 3/60.0 };

    constexpr float PI{ 3.14159265358979323 };
    constexpr float TAU{ 6.283185307179586 }; // 2 * PI
    constexpr float RADIANS_TO_DEGREES_CONVERSION_CONSTANT{ 57.29577951308232 }; // 180 / PI
    constexpr float DEGREES_TO_RADIANS_CONVERSION_CONSTANT{ 0.017453292519943295 }; // PI / 180

    const sf::Color SKY_BLUE{ sf::Color(192, 211, 234) };
}

#endif