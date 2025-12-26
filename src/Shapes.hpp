#pragma once

#include "raymath.h"
#include <vector>

/**
 * Vrací definici tvaru tetromina pro daný typ a rotaci.
 *
 * Obsahuje všech 7 standardních Tetris tvarů s 4 rotacemi každý:
 * - 0: O (čtverec) - všechny rotace jsou stejné
 * - 1: I (linka) - horizontální a vertikální
 * - 2: T (tvar T)
 * - 3: S (klikatice doprava)
 * - 4: Z (klikatice doleva)
 * - 5: L (L doleva)
 * - 6: J (L doprava)
 *
 * Každý tvar je definován jako pole 4 Vector2 pozic představujících
 * buňky tetromina v lokálních souřadnicích (0-3).
 *
 * @param shape_type Typ tvaru (0-6)
 * @param rotation Rotace (0-3, kde 0 = 0°, 1 = 90°, 2 = 180°, 3 = 270°)
 * @return Vektor 4 pozic buněk tvořících tetromino
 */
inline std::vector<Vector2> GetShape(int shape_type, int rotation) {
    static const Vector2 SHAPES[7][4][4] = {
        // 0: O - čtverec (všechny rotace identické)
        {
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
        },
        // 1: I - linka (horizontální ↔ vertikální)
        {
            {{0, 1}, {1, 1}, {2, 1}, {3, 1}},  // Horizontální
            {{2, 0}, {2, 1}, {2, 2}, {2, 3}},  // Vertikální
            {{0, 2}, {1, 2}, {2, 2}, {3, 2}},  // Horizontální
            {{1, 0}, {1, 1}, {1, 2}, {1, 3}}   // Vertikální
        },
        // 2: T - tvar T
        {
            {{1, 0}, {0, 1}, {1, 1}, {2, 1}},  // T nahoru
            {{1, 0}, {1, 1}, {2, 1}, {1, 2}},  // T vpravo
            {{0, 1}, {1, 1}, {2, 1}, {1, 2}},  // T dolů
            {{1, 0}, {0, 1}, {1, 1}, {1, 2}}   // T vlevo
        },
        // 3: S - klikatice (směrem doprava)
        {
            {{1, 0}, {2, 0}, {0, 1}, {1, 1}},
            {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
            {{1, 1}, {2, 1}, {0, 2}, {1, 2}},
            {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
        },
        // 4: Z - klikatice (směrem doleva)
        {
            {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
            {{2, 0}, {1, 1}, {2, 1}, {1, 2}},
            {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
            {{1, 0}, {0, 1}, {1, 1}, {0, 2}}
        },
        // 5: L - tvar L (roh vlevo)
        {
            {{2, 0}, {0, 1}, {1, 1}, {2, 1}},
            {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
            {{0, 1}, {1, 1}, {2, 1}, {0, 2}},
            {{0, 0}, {1, 0}, {1, 1}, {1, 2}}
        },
        // 6: J - tvar J (roh vpravo)
        {
            {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
            {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
            {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
            {{1, 0}, {1, 1}, {0, 2}, {1, 2}}
        }
    };

    // Sestavení výsledného vektoru ze statické definice
    std::vector<Vector2> result;
    for (int i = 0; i < 4; i++) {
        result.push_back(SHAPES[shape_type][rotation][i]);
    }
    return result;
}
