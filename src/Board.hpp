#pragma once

#include "raylib.h"
#include "Particle.hpp"
#include "ExplosionParticle.hpp"
#include "TrailParticle.hpp"
#include <vector>
#include <set>
#include <unordered_map>

class Board {
public:
    int width, height;
    std::vector<Particle*> particles;
    std::vector<ExplosionParticle> explosion_particles;
    std::vector<TrailParticle> trail_particles;
    std::vector<std::vector<Particle*>> grid;
    int shake_amount;
    int shake_duration;
    int frame_counter;
    int dir_index;
    RenderTexture2D background_texture;

    int gravity_counter;
    bool grid_dirty;
    int surrounded_check_counter;

    // Pre-explosion animace
    enum class ExplosionState {
        NONE,
        ZOOMING,
        EXPLODING
    };
    ExplosionState explosion_state;
    int explosion_timer;
    std::set<Particle*> particles_to_explode;
    std::unordered_map<Particle*, float> particle_scale_factors;
    Color explosion_flash_color;

    Board();
    ~Board();

    void CreateBackground();
    void AddParticles(const std::vector<Particle>& new_particles);
    void TriggerShake(int intensity = 10);
    void UpdateShake();
    Vector2 GetShakeOffset();
    bool CheckCollision(const std::vector<Particle>& test_particles);
    void RebuildGrid();
    void ApplyGravity();
    std::set<Particle*> FindConnectedGroup(Particle* start);
    int CheckHorizontalConnections();
    void UpdatePreExplosionAnimation();
    void UpdateExplosions();
    void AddTrailsForMovingParticles();
    void UpdateTrails();
    void Draw(int offset_x, int offset_y);
};
