#pragma once

#include "raylib.h"

class TrailParticle {
public:
    float x, y;
    Color color;
    int lifetime;
    int age;
    float size;

    TrailParticle(int x, int y, Color color);
    void Update();
    bool IsAlive() const;
    void Draw(int offset_x, int offset_y);
};
