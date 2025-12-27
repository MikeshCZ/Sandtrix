#include "Tetromino.hpp"
#include "Constants.hpp"
#include "Shapes.hpp"
#include <random>

// Konstruktor - vytvoří náhodné tetromino
Tetromino::Tetromino(int board_x, int board_y)
    : board_x(board_x), board_y(board_y), rotation(0), is_active(true) {
    // Náhodný výběr tvaru (0-6: O, I, T, S, Z, L, J)
    std::uniform_int_distribution<> shape_dist(0, 6);
    // Náhodný výběr barvy z dostupných barev (podle obtížnosti)
    std::uniform_int_distribution<> color_dist(0, NUM_COLORS - 1);

    shape_type = shape_dist(gen);
    color = ALL_COLORS[color_dist(gen)];
    GenerateParticles();
}

// Generuje částice pro aktuální tvar a rotaci
void Tetromino::GenerateParticles() {
    particles.clear();
    auto shape = GetShape(shape_type, rotation);

    // Pro každou buňku tvaru vytvořit mřížku 5×5 částic
    for (auto& block : shape) {
        int bx = (int)block.x;
        int by = (int)block.y;
        for (int px = 0; px < PARTICLES_PER_BLOCK; px++) {
            for (int py = 0; py < PARTICLES_PER_BLOCK; py++) {
                particles.emplace_back(
                    (board_x + bx) * PARTICLES_PER_BLOCK + px,
                    (board_y + by) * PARTICLES_PER_BLOCK + py,
                    color
                );
            }
        }
    }
}

// Posun tetromina o dx, dy buněk
void Tetromino::Move(int dx, int dy) {
    board_x += dx;
    board_y += dy;
    // Posunout všechny částice
    for (auto& p : particles) {
        p.x += dx * PARTICLES_PER_BLOCK;
        p.y += dy * PARTICLES_PER_BLOCK;
    }
}

// Rotace tetromina o 90° doprava
void Tetromino::Rotate() {
    rotation = (rotation + 1) % 4;
    GenerateParticles(); // Regenerovat částice pro novou rotaci
}

// Vykreslit tetromino s enhanced efektem
void Tetromino::Draw(int offset_x, int offset_y) {
    for (auto& p : particles) {
        p.Draw(offset_x, offset_y, true); // true = enhanced rendering
    }
}
