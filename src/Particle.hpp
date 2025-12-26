#pragma once

#include "raylib.h"

/**
 * Základní písková částice s fyzikou.
 * Reprezentuje jeden pixel v systému částic, může padat,
 * kolizovat s jinými částicemi a být součástí výbuchů.
 * Velikost částice je 8×8 pixelů.
 */
class Particle {
public:
    float x, y;                 // Pozice částice v pixelech
    Color color;                // Barva částice
    int velocity_y;             // Vertikální rychlost (pro gravitaci)
    bool settled;               // Příznak, zda částice usedla (nepohybuje se)
    bool fully_surrounded;      // Příznak, zda je obklopena ze všech stran (optimalizace)
    float brightness;           // Náhodná variace jasu (0.8 - 1.2)

    /**
     * Konstruktor - vytvoří novou částici.
     * @param x Horizontální pozice v pixelech
     * @param y Vertikální pozice v pixelech
     * @param color Barva částice
     */
    Particle(float x, float y, Color color);

    /**
     * Vykreslí částici na obrazovku s možnými efekty.
     * @param offset_x Horizontální offset na obrazovce
     * @param offset_y Vertikální offset na obrazovce
     * @param enhanced Pokud true, vykreslí s vylepšeným vizuálním efektem
     * @param scale Škálovací faktor velikosti (1.0 = normální)
     */
    void Draw(int offset_x, int offset_y, bool enhanced = false, float scale = 1.0f);
};
