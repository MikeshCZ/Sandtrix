#pragma once

#include "raylib.h"
#include <random>

// Konstanty
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 900;
constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;
constexpr int PARTICLE_SIZE = 8;
constexpr int PARTICLES_PER_BLOCK = 5;
constexpr int CELL_SIZE = PARTICLE_SIZE * PARTICLES_PER_BLOCK;
constexpr int FPS = 60;
constexpr int FALL_SPEED = 50;
constexpr int MOVE_DELAY = 8;

// Barvy - modernější paleta s vibrantními odstíny
const Color ALL_COLORS[] = {
    {255, 71, 87, 255},    // Koralově červená
    {46, 213, 115, 255},   // Mentolově zelená
    {72, 219, 251, 255},   // Jasně tyrkysová
    {255, 195, 18, 255},   // Zlatě žlutá
    {162, 155, 254, 255},  // Světle fialová
    {255, 107, 129, 255}   // Růžová
};

constexpr Color BG_COLOR_TOP = {20, 20, 35, 255};
constexpr Color BG_COLOR_BOTTOM = {40, 20, 50, 255};
constexpr Color DARK_GRAY = {30, 30, 40, 255};

// Globální proměnné
extern int NUM_COLORS;
extern bool MUSIC_ENABLED;
extern std::mt19937 gen;
