#pragma once

#include "raylib.h"

/**
 * Smíchá dvě barvy podle zadaného poměru.
 * @param c1 První barva
 * @param c2 Druhá barva
 * @param blend Poměr směsi (0.0 = pouze c1, 1.0 = pouze c2)
 * @return Výsledná smíchaná barva
 */
Color BlendColors(Color c1, Color c2, float blend);

/**
 * Zesvětlí barvu podle zadaného faktoru.
 * @param c Původní barva
 * @param factor Faktor zesvětlení (1.0 = beze změny, >1.0 = světlejší)
 * @return Zesvětlená barva
 */
Color BrightenColor(Color c, float factor);

/**
 * Ztmavne barvu podle zadaného faktoru.
 * @param c Původní barva
 * @param factor Faktor ztmavnutí (1.0 = beze změny, <1.0 = tmavší)
 * @return Ztmavlá barva
 */
Color DarkenColor(Color c, float factor);

/**
 * Vytvoří novou barvu se změněnou průhledností.
 * @param c Původní barva
 * @param alpha Nová hodnota alfa kanálu (0-255)
 * @return Barva s upravenou průhledností
 */
Color ColorWithAlpha(Color c, unsigned char alpha);
