#include "TrailParticle.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include <random>

TrailParticle::TrailParticle(int x, int y, Color color)
    : x((float)x), y((float)y), color(color), age(0) {
    std::uniform_int_distribution<> lifetime_dist(10, 20);
    std::uniform_real_distribution<> size_dist(1.5, 3.0);
    lifetime = lifetime_dist(gen);
    size = size_dist(gen);
}

void TrailParticle::Update() {
    age++;
}

bool TrailParticle::IsAlive() const {
    return age < lifetime;
}

void TrailParticle::Draw(int offset_x, int offset_y) {
    float life_ratio = 1.0f - (float)age / lifetime;
    unsigned char alpha = (unsigned char)(150 * life_ratio);

    int x_pos = offset_x + (int)(x * PARTICLE_SIZE);
    int y_pos = offset_y + (int)(y * PARTICLE_SIZE);
    int current_size = (int)(size * life_ratio);

    if (current_size > 0) {
        Color fade_color = DarkenColor(color, 0.8f);
        fade_color.a = alpha;
        DrawCircle(x_pos, y_pos, (float)current_size, fade_color);
    }
}
