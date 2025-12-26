#pragma once

#include "raylib.h"

/**
 * Částice výbuchového efektu.
 * Vytváří se při výbuchu propojených skupin a létá směrem od centra exploze.
 * Má omezenou životnost a postupně mizí (fade out efekt).
 */
class ExplosionParticle {
public:
    float x, y, vx, vy;          // Pozice a rychlost částice
    Color color;                 // Barva částice
    int lifetime, age;           // Maximální a aktuální věk v framech
    float size;                  // Velikost částice
    float rotation;              // Aktuální rotace v radiánech
    float rotation_speed;        // Rychlost rotace

    /**
     * Konstruktor - vytvoří novou výbuchovou částici s náhodnou rychlostí.
     * @param x Počáteční x pozice
     * @param y Počáteční y pozice
     * @param color Barva částice
     */
    ExplosionParticle(int x, int y, Color color);

    /**
     * Aktualizuje pozici, rotaci a stárnutí částice.
     * Aplikuje gravitaci na vertikální rychlost.
     */
    void Update();

    /**
     * Zjistí, zda částice ještě existuje.
     * @return true pokud věk < životnost
     */
    bool IsAlive() const;

    /**
     * Vykreslí částici s fade out efektem podle věku.
     * @param offset_x Horizontální offset na obrazovce
     * @param offset_y Vertikální offset na obrazovce
     */
    void Draw(int offset_x, int offset_y);
};
