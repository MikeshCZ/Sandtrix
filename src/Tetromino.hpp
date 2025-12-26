#pragma once

#include "raylib.h"
#include "Particle.hpp"
#include <vector>

class Tetromino {
public:
    int shape_type;
    int rotation;
    Color color;
    int board_x, board_y;
    std::vector<Particle> particles;
    bool is_active;

    Tetromino(int board_x, int board_y);
    void GenerateParticles();
    void Move(int dx, int dy);
    void Rotate();
    void Draw(int offset_x, int offset_y);
};
