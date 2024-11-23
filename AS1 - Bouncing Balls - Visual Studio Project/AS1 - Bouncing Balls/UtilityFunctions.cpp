#include <string>
#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Registry.h"

using namespace std;

float pythagorean_distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

float random_float(float minimum, float maximum) {
    float random = static_cast<float>(rand()) / RAND_MAX;
    return minimum + random * (maximum - minimum);
}

int calculate_point_count(int radius, float shape_quality) {
    if (radius < 1) {
        return 3;
    }
    float angle = asin(1.0f / radius);
    int point_count = 1 + int((Constants::TAU / angle) * shape_quality);

    if (point_count < 3) {
        point_count = 3;
    }

    return point_count;
}

string path_builder(string(&args)[50]) { // currently length 50, lower
    // this to match longest path for memory savings.

    string resultant_path = Constants::PROJECT_RESOURCES_DIRECTORY;
    int length_of_input_array = sizeof(args) / sizeof(args[0]);
    for (int i = 0; i < length_of_input_array; i++) {
        if (args[i] == "") {
            break;
        }
        resultant_path += "\\" + args[i];
    }
    return resultant_path;
}

void play_random_pop_sounds(int count) {
    for (int i = 0; i < count; i++) {
        int sound_index = rand() % 15;
        Registry::pop_sounds[sound_index].play();
    }
    // it is possible to do this serially. By creating custom music objects 
    // with a play function that mimics the music.play function and then 
    // updating these objects every frame with custom delays.
    // Note: doing this would require that the audio isn't already in its 
    // delay state which could easily be added in. This limits max number 
    // of sounds to 15, as that's how many unique files we have,
    // however clever sound design should be able to mask this limitation.
}

void play_cannon_fire_sound() {
    Registry::cannon_fire_sound.stop();
    //Registry::cannon_fire_sound.play();
}

sf::Color pick_ball_color() {
    int randomly_chosen_color = rand() % 3;
    if (randomly_chosen_color == 0) {
        return sf::Color::Blue;
    }
    else if (randomly_chosen_color == 1) {
        return sf::Color::Yellow;
    }
    else {
        return sf::Color::Red;
    }
}