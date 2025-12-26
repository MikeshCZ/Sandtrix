#pragma once

#include "raylib.h"

class Particle {
public:
    float x, y;
    Color color;
    int velocity_y;
    bool settled;
    bool fully_surrounded;  // OPTIMALIZACE: pokud je obklopená ze všech stran
    float brightness;

    Particle(float x, float y, Color color);
    void Draw(int offset_x, int offset_y, bool enhanced = false, float scale = 1.0f);
};
