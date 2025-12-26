#include "Tetromino.hpp"
#include "Constants.hpp"
#include "Shapes.hpp"
#include <random>

Tetromino::Tetromino(int board_x, int board_y)
    : board_x(board_x), board_y(board_y), rotation(0), is_active(true) {
    std::uniform_int_distribution<> shape_dist(0, 6);
    std::uniform_int_distribution<> color_dist(0, NUM_COLORS - 1);

    shape_type = shape_dist(gen);
    color = ALL_COLORS[color_dist(gen)];
    GenerateParticles();
}

void Tetromino::GenerateParticles() {
    particles.clear();
    auto shape = GetShape(shape_type, rotation);

    // OPTIMALIZACE: Sníženo z 10x10 na 5x5 částic na blok = 4x méně částic!
    constexpr int PARTICLES_PER_BLOCK = 5;  // Bylo 10, nyní 5
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

void Tetromino::Move(int dx, int dy) {
    board_x += dx;
    board_y += dy;
    for (auto& p : particles) {
        p.x += dx * PARTICLES_PER_BLOCK;
        p.y += dy * PARTICLES_PER_BLOCK;
    }
}

void Tetromino::Rotate() {
    rotation = (rotation + 1) % 4;
    GenerateParticles();
}

void Tetromino::Draw(int offset_x, int offset_y) {
    for (auto& p : particles) {
        p.Draw(offset_x, offset_y, true);
    }
}
