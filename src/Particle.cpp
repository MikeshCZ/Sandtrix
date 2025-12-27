#include "Particle.hpp"
#include "Constants.hpp"
#include "Utils.hpp"

// Konstruktor - vytvoří základní částici
Particle::Particle(float x, float y, Color color)
    : x(x), y(y), color(color), velocity_y(0), settled(false), brightness(1.0f) {}

// Vykreslí částici s možnými efekty (enhanced mode pro padající tetromino, scale pro výbuchy)
void Particle::Draw(int offset_x, int offset_y, bool enhanced, float scale) {
    // Vypočítat pixel pozici
    int base_x = offset_x + (int)x * PARTICLE_SIZE;
    int base_y = offset_y + (int)y * PARTICLE_SIZE;

    // Aplikovat škálování (pro zoom animaci při výbuchu)
    int scaled_size = (int)(PARTICLE_SIZE * scale);
    int size_diff = PARTICLE_SIZE - scaled_size;
    int x_pos = base_x + size_diff / 2;
    int y_pos = base_y + size_diff / 2;

    if (enhanced) {
        // Enhanced rendering pro aktivní tetromino
        Color inner = BrightenColor(color, 1.2f + (scale - 1.0f) * 0.5f);
        DrawRectangle(x_pos, y_pos, scaled_size, scaled_size, inner);
        Color bright = BrightenColor(color, 1.4f + (scale - 1.0f) * 0.8f);
        DrawRectangleLines(x_pos, y_pos, scaled_size, scaled_size, bright);

        // Glow efekt pro velké škálování (při výbuchu)
        if (scale > 1.2f) {
            Color glow = color;
            glow.a = (unsigned char)(80 * (scale - 1.0f));
            DrawRectangle(x_pos - 2, y_pos - 2, scaled_size + 4, scaled_size + 4, glow);
        }
    } else {
        // Normální rendering pro částice na desce
        DrawRectangle(x_pos, y_pos, scaled_size, scaled_size, color);

        // Highlight (světlá linie nahoře a vlevo)
        Color highlight = BrightenColor(color, 1.3f);
        DrawLine(x_pos, y_pos, x_pos + scaled_size - 1, y_pos, highlight);
        DrawLine(x_pos, y_pos, x_pos, y_pos + scaled_size - 1, highlight);

        // Shadow (tmavá linie dole a vpravo) - 3D efekt
        Color shadow = DarkenColor(color, 0.7f);
        DrawLine(x_pos, y_pos + scaled_size - 1, x_pos + scaled_size - 1, y_pos + scaled_size - 1, shadow);
        DrawLine(x_pos + scaled_size - 1, y_pos, x_pos + scaled_size - 1, y_pos + scaled_size - 1, shadow);
    }
}
