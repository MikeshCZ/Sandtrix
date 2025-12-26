#pragma once

#include "raylib.h"

class ExplosionParticle {
public:
    float x, y;
    float vx, vy;
    Color color;
    int lifetime;
    int age;
    float size;
    float rotation;
    float rotation_speed;

    ExplosionParticle(int x, int y, Color color);
    void Update();
    bool IsAlive() const;
    void Draw(int offset_x, int offset_y);
};
