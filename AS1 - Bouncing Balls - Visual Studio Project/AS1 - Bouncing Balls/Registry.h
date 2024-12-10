/*
This program stores all the globally accessed variables used throughout the Bouncing Balls program. This is
in principle somewhat similar to the constants, except that these values can be changed during runtime by any
reference to the name. It should be noted that here variables are only declared, and it not until the 'registry.cpp'
file that these variables are defined any initial values.
*/
#pragma once
#pragma warning( disable : 4244 )

#include <SFML/Audio.hpp>
#include <string>

using namespace std;

#ifndef REGISTRY_H
#define REGISTRY_H
// here the Registry namespace is defined much like how the Constants namespace was defined, except here it is important
// that each program accesses the SAME shared registry, so it appears to behave more like a globally accessible catalogue
// this is where the 'ifndef' comes in as it checks to see if the namespace REGISTRY_H already exists, and will use that instead
// of recreating it again.

namespace Registry {
	extern int window_size[2]; // here an array of two values is defined and it stores the X and Y size in the format;
	// window_size[] = {X, Y};
	// and this variable is used in almost every component of the Bouncing Balls game as it acts as an easy way to get access
	// to the current window size for positioning or moving content around on-screen. This was initially used in this way to help
	// with resizing the game window, but this idea was later scrapped because it was deemed too technically complex to be doing in
	// a level, as it would cause all the balls to need their positions, points and radii changed, which can caused a great deal of
	// additional complications, not to mention that the mass fall speed which would also need to be re-determined as it is dependant
	// on the size of the window to achieve the desired 1 minute and 30 second game duration.
	extern string game_end_state; // this simply stores if the player has won or lost the game, which is important to keep track of
	// for the end menu state.
	extern int ball_radius; // this is used to determine the radius of the balls seen in the game levels. This was here initially because
	// the game was initially meant to include the ability to resize the window, at which point this value would also change, however this idea
	// was scrapped, and given the value changes on window size, it remains constant now for the duration of the program once initially set.
	extern float ball_tesselation_coefficient; // this is used to determine mathematically how to reduce the spacing between each row of
	// balls in each level, and ensures that the rows properly interlock, which is vital for the flag spreading algorithms used in later mechanics
	// as well as for collision detection and graphical improvements.
	extern int score; // this is used to keep track of the players score as they move through each level. For each ball that they drop, 50
	// points is added to this variable, and the resultant score shown on the end menu.
	extern float run_time; // this is used as a method of keeping track of now long the game loop has been running for, with each frame being
	// exactly 1/60th of a second and thus that is what this variable is incremented by each frame. This is very important for ensuring linear
	// interpolations and game physics run smoothly.
}

#endif