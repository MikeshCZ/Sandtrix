#pragma once

#include "raylib.h"
#include <random>

// =============================================================================
// Herní konstanty - rozměry a timing
// =============================================================================

constexpr int SCREEN_WIDTH = 800;              // Šířka herního okna v pixelech
constexpr int SCREEN_HEIGHT = 900;             // Výška herního okna v pixelech
constexpr int BOARD_WIDTH = 10;                // Šířka desky v buňkách
constexpr int BOARD_HEIGHT = 20;               // Výška desky v buňkách
constexpr int PARTICLE_SIZE = 8;               // Velikost jedné částice v pixelech
constexpr int PARTICLES_PER_BLOCK = 5;         // Počet částic na stranu buňky (5×5 = 25 částic)
constexpr int CELL_SIZE = PARTICLE_SIZE * PARTICLES_PER_BLOCK;  // Velikost buňky (40px)
constexpr int FPS = 60;                        // Cílový počet snímků za sekundu
constexpr int FALL_SPEED = 50;                 // Rychlost pádu tetromina (framů na posun)
constexpr int MOVE_DELAY = 8;                  // Zpoždění pro plynulý pohyb při držení klávesy

// =============================================================================
// Barevná paleta
// =============================================================================

const Color ALL_COLORS[] = {
    {255, 71, 87, 255},    // Červená
    {46, 213, 115, 255},   // Zelená
    {72, 219, 251, 255},   // Azurová
    {255, 195, 18, 255},   // Žlutá
    {162, 155, 254, 255},  // Fialová
    {255, 107, 129, 255}   // Růžová
};

constexpr Color BG_COLOR_TOP = {20, 20, 35, 255};       // Horní barva pozadí
constexpr Color BG_COLOR_BOTTOM = {40, 20, 50, 255};    // Spodní barva pozadí
constexpr Color DARK_GRAY = {30, 30, 40, 255};          // Tmavě šedá pro UI

// =============================================================================
// Globální proměnné
// =============================================================================

extern int NUM_COLORS;              // Počet dostupných barev (vypočítá se při běhu)
extern bool MUSIC_ENABLED;          // Přepínač pro hudbu (nastavitelné v menu)
extern std::mt19937 gen;            // Generátor náhodných čísel
