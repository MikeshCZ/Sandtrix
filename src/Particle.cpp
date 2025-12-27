#include "Particle.hpp"
#include "Constants.hpp"
#include "Utils.hpp"

Particle::Particle(float x, float y, Color color)
    : x(x), y(y), color(color), velocity_y(0), settled(false), brightness(1.0f) {}

void Particle::Draw(int offset_x, int offset_y, bool enhanced, float scale) {
    int base_x = offset_x + (int)x * PARTICLE_SIZE;
    int base_y = offset_y + (int)y * PARTICLE_SIZE;

    int scaled_size = (int)(PARTICLE_SIZE * scale);
    int size_diff = PARTICLE_SIZE - scaled_size;
    int x_pos = base_x + size_diff / 2;
    int y_pos = base_y + size_diff / 2;

    if (enhanced) {
        Color inner = BrightenColor(color, 1.2f + (scale - 1.0f) * 0.5f);
        DrawRectangle(x_pos, y_pos, scaled_size, scaled_size, inner);
        Color bright = BrightenColor(color, 1.4f + (scale - 1.0f) * 0.8f);
        DrawRectangleLines(x_pos, y_pos, scaled_size, scaled_size, bright);

        if (scale > 1.2f) {
            Color glow = color;
            glow.a = (unsigned char)(80 * (scale - 1.0f));
            DrawRectangle(x_pos - 2, y_pos - 2, scaled_size + 4, scaled_size + 4, glow);
        }
    } else {
        DrawRectangle(x_pos, y_pos, scaled_size, scaled_size, color);

        Color highlight = BrightenColor(color, 1.3f);
        DrawLine(x_pos, y_pos, x_pos + scaled_size - 1, y_pos, highlight);
        DrawLine(x_pos, y_pos, x_pos, y_pos + scaled_size - 1, highlight);

        Color shadow = DarkenColor(color, 0.7f);
        DrawLine(x_pos, y_pos + scaled_size - 1, x_pos + scaled_size - 1, y_pos + scaled_size - 1, shadow);
        DrawLine(x_pos + scaled_size - 1, y_pos, x_pos + scaled_size - 1, y_pos + scaled_size - 1, shadow);
    }
}
