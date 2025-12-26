#include "ExplosionParticle.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include <random>

ExplosionParticle::ExplosionParticle(int x, int y, Color color)
    : x((float)x), y((float)y), color(color), age(0) {
    std::uniform_real_distribution<> speed_dist(2.5, 6.0);
    std::uniform_real_distribution<> angle_dist(-1.0, 1.0);
    std::uniform_real_distribution<> jump_dist(2.0, 5.0);
    std::uniform_int_distribution<> lifetime_dist(20, 40);
    std::uniform_real_distribution<> size_dist(2.0, 4.5);
    std::uniform_real_distribution<> rot_dist(-15.0, 15.0);
    std::uniform_real_distribution<> angle_360(0.0, 360.0);

    float speed = speed_dist(gen);
    vx = speed * angle_dist(gen);
    vy = speed * angle_dist(gen) - jump_dist(gen);
    lifetime = lifetime_dist(gen);
    size = size_dist(gen);
    rotation = angle_360(gen);
    rotation_speed = rot_dist(gen);
}

void ExplosionParticle::Update() {
    x += vx;
    y += vy;
    vy += 0.4f;
    vx *= 0.98f;
    age++;
    rotation += rotation_speed;
}

bool ExplosionParticle::IsAlive() const {
    return age < lifetime;
}

void ExplosionParticle::Draw(int offset_x, int offset_y) {
    float life_ratio = 1.0f - (float)age / lifetime;
    unsigned char alpha = (unsigned char)(150 * life_ratio);

    Color bright_color;
    if (life_ratio > 0.7f) {
        float factor = (life_ratio - 0.7f) / 0.3f;
        bright_color = BrightenColor(color, 1.0f + 0.3f * factor);
    } else {
        bright_color = BrightenColor(color, 0.6f + life_ratio * 0.4f);
    }

    int x_pos = offset_x + (int)(x * PARTICLE_SIZE);
    int y_pos = offset_y + (int)(y * PARTICLE_SIZE);
    int current_size = (int)(size * (0.5f + life_ratio * 0.5f));

    if (current_size > 0) {
        bright_color.a = alpha;
        DrawCircle(x_pos, y_pos, (float)current_size, bright_color);
    }
}
