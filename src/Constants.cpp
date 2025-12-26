#include "Constants.hpp"

// Globální proměnné
int NUM_COLORS = 4;  // Výchozí = normální obtížnost
bool MUSIC_ENABLED = true;  // Výchozí = hudba zapnutá

std::random_device rd;
std::mt19937 gen(rd());
