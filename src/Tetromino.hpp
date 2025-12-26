#pragma once

#include "raylib.h"
#include "Particle.hpp"
#include <vector>

/**
 * Padající tetromino kostka složená z částic.
 * Implementuje 7 standardních Tetris tvarů (O, I, T, S, Z, L, J)
 * s 4 rotacemi každý. Každý tvar je složen ze 4 buněk, každá buňka
 * obsahuje 5×5 = 25 částic, celkem 100 částic na tetromino.
 */
class Tetromino {
public:
    int shape_type, rotation;         // Typ tvaru (0-6) a rotace (0-3)
    Color color;                      // Barva tetromina
    int board_x, board_y;             // Pozice na desce (v buňkách)
    std::vector<Particle> particles;  // Všechny částice tvořící tetromino
    bool is_active;                   // Příznak, zda tetromino stále padá

    /**
     * Konstruktor - vytvoří nové tetromino náhodného tvaru a barvy.
     * @param board_x Počáteční x pozice na desce
     * @param board_y Počáteční y pozice na desce
     */
    Tetromino(int board_x, int board_y);

    /**
     * Generuje všechny částice pro aktuální tvar a rotaci.
     * Vytváří mřížku 5×5 částic pro každou buňku tetromina.
     */
    void GenerateParticles();

    /**
     * Posune tetromino o zadaný offset.
     * @param dx Posun v ose x (v buňkách)
     * @param dy Posun v ose y (v buňkách)
     */
    void Move(int dx, int dy);

    /**
     * Rotuje tetromino o 90° po směru hodinových ručiček.
     * Po rotaci regeneruje částice v nové pozici.
     */
    void Rotate();

    /**
     * Vykreslí tetromino na obrazovku.
     * @param offset_x Horizontální offset na obrazovce
     * @param offset_y Vertikální offset na obrazovce
     */
    void Draw(int offset_x, int offset_y);
};
