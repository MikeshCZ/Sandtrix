#include "Board.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <deque>
#include <cmath>

Board::Board() : width(BOARD_WIDTH * PARTICLES_PER_BLOCK), height(BOARD_HEIGHT * PARTICLES_PER_BLOCK),
          shake_amount(0), shake_duration(0), frame_counter(0), dir_index(0),
          gravity_counter(0), grid_dirty(true), surrounded_check_counter(0),
          explosion_state(ExplosionState::NONE), explosion_timer(0),
          explosion_flash_color(WHITE) {
    grid.resize(height, std::vector<Particle*>(width, nullptr));
    CreateBackground();
}

Board::~Board() {
    UnloadRenderTexture(background_texture);
    for (auto p : particles) delete p;
}

void Board::CreateBackground() {
    int board_width_px = width * PARTICLE_SIZE;
    int board_height_px = height * PARTICLE_SIZE;
    background_texture = LoadRenderTexture(board_width_px, board_height_px);

    BeginTextureMode(background_texture);
    ClearBackground(BLACK);

    // Gradient pozadí
    for (int y = 0; y < board_height_px; y++) {
        float blend = (float)y / board_height_px;
        Color bg_color = {
            (unsigned char)(10 + 5 * blend),
            (unsigned char)(10 + 5 * blend),
            (unsigned char)(15 + 10 * blend),
            255
        };
        DrawLine(0, y, board_width_px, y, bg_color);
    }

    // Mřížka
    Color grid_color = {40, 40, 55, 255};
    for (int i = 0; i <= BOARD_WIDTH; i++) {
        Color gc = (i % 2 == 0) ? Color{50, 50, 65, 255} : Color{40, 40, 55, 255};
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, board_height_px, gc);
    }
    for (int i = 0; i <= BOARD_HEIGHT; i++) {
        DrawLine(0, i * CELL_SIZE, board_width_px, i * CELL_SIZE, grid_color);
    }

    EndTextureMode();
}

void Board::AddParticles(const std::vector<Particle>& new_particles) {
    for (const auto& p : new_particles) {
        if (p.y >= 0 && p.y < height && p.x >= 0 && p.x < width) {
            particles.push_back(new Particle(p));
        }
    }
}

void Board::TriggerShake(int intensity) {
    // Pokud už třesení probíhá, přičteme intenzitu místo nahrazení
    if (shake_amount > 0) {
        shake_amount = std::min(30, shake_amount + intensity);
        shake_duration = std::max(shake_duration, 30);
    } else {
        shake_amount = intensity;
        shake_duration = 30;
    }
}

void Board::UpdateShake() {
    if (shake_duration > 0) {
        shake_duration--;
        shake_amount = (int)(shake_amount * 0.92f);
    } else {
        shake_amount = 0;
    }
}

Vector2 Board::GetShakeOffset() {
    if (shake_amount > 0) {
        std::uniform_int_distribution<> shake_dist(-shake_amount, shake_amount);
        return {(float)shake_dist(gen), (float)shake_dist(gen)};
    }
    return {0, 0};
}

bool Board::CheckCollision(const std::vector<Particle>& test_particles) {
    for (const auto& p : test_particles) {
        int x = (int)p.x;
        int y = (int)p.y;

        if (x < 0 || x >= width || y >= height) return true;
        if (y >= 0 && grid[y][x] != nullptr) return true;
    }
    return false;
}

void Board::RebuildGrid() {
    // Vyčistit grid
    for (auto& row : grid) {
        std::fill(row.begin(), row.end(), nullptr);
    }

    // Naplnit grid
    for (auto p : particles) {
        int px = (int)p->x;
        int py = (int)p->y;
        if (py >= 0 && py < height && px >= 0 && px < width) {
            grid[py][px] = p;
        }
    }
    grid_dirty = false;
}

void Board::ApplyGravity() {
    if (grid_dirty) {
        RebuildGrid();
    }

    // Zkontrolovat usazené částice - OPTIMALIZACE: jen pokud jsou nějaké neusazené
    bool has_unsettled = false;
    for (auto p : particles) {
        if (!p->settled) {
            has_unsettled = true;
            break;
        }
    }

    // Pokud jsou všechny usazené, nic nedělat
    if (!has_unsettled) return;

    // OPTIMALIZACE: Označit plně obklopené částice - JEN KAŽDÝCH 10 FRAMŮ
    surrounded_check_counter++;
    if (surrounded_check_counter >= 10) {
        surrounded_check_counter = 0;

        for (auto p : particles) {
            if (!p->settled) {
                p->fully_surrounded = false;
                continue;
            }

            int px = (int)p->x;
            int py = (int)p->y;

            if (px <= 0 || px >= width - 1 || py <= 0 || py >= height - 1) {
                // Částice na kraji není plně obklopená
                p->fully_surrounded = false;
                continue;
            }

            // Zkontrolovat všech 8 sousedů
            bool surrounded = true;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = px + dx;
                    int ny = py + dy;
                    if (grid[ny][nx] == nullptr) {
                        surrounded = false;
                        break;
                    }
                }
                if (!surrounded) break;
            }
            p->fully_surrounded = surrounded;
        }
    }

    // Zkontrolovat usazené částice nad neusazenými - ale skip plně obklopené
    for (auto p : particles) {
        if (p->fully_surrounded) continue;

        int px = (int)p->x;
        int py = (int)p->y;
        if (p->settled && py >= 0 && py + 1 < height && px >= 0 && px < width) {
            if (grid[py + 1][px] == nullptr) {
                p->settled = false;
                p->fully_surrounded = false;
            }
        }
    }

    // Seřadit neusazené částice - SKIP plně obklopené
    std::vector<Particle*> unsettled;
    for (auto p : particles) {
        if (!p->settled && !p->fully_surrounded) {
            unsettled.push_back(p);
        }
    }

    // OPTIMALIZACE: omezit počet zpracovávaných částic na max 1000 za frame
    if (unsettled.size() > 1000) {
        unsettled.resize(1000);
    }

    std::sort(unsettled.begin(), unsettled.end(),
              [](Particle* a, Particle* b) { return a->y > b->y; });

    dir_index = (dir_index + 1) % 2;
    int diagonal_dirs[][2] = {{-1, 1}, {1, -1}};
    int horizontal_dirs[][2] = {{1, -1}, {-1, 1}};

    // Zpracovat částice - rychlejší pád
    for (auto particle : unsettled) {
        int old_x = (int)particle->x;
        int old_y = (int)particle->y;
        bool moved = false;

        if (old_y >= 0 && old_y < height && old_x >= 0 && old_x < width) {
            grid[old_y][old_x] = nullptr;
        }

        // Zvýšená rychlost pádu - zkusit spadnout o více pixelů najednou
        particle->velocity_y++;
        int fall_distance = std::min(particle->velocity_y / 2 + 1, 3);

        int new_y = old_y;
        for (int step = 0; step < fall_distance; step++) {
            int test_y = old_y + step + 1;
            if (test_y >= height || (test_y >= 0 && grid[test_y][old_x] != nullptr)) {
                break;
            }
            new_y = test_y;
        }

        if (new_y > old_y) {
            // Částice spadla
            particle->y = new_y;
            if (new_y >= height - 1) {
                particle->settled = true;
                particle->velocity_y = 0;
                particle->y = height - 1;
                grid[(int)particle->y][(int)particle->x] = particle;
            } else {
                grid[new_y][old_x] = particle;
            }
            moved = true;
        } else if (old_y + 1 >= height) {
            particle->settled = true;
            particle->velocity_y = 0;
            particle->y = height - 1;
            grid[(int)particle->y][(int)particle->x] = particle;
            moved = true;
        } else {
            // Nemůže spadnout dolů, zkusit diagonálně
            particle->velocity_y = 0;
            int test_y = old_y + 1;

            // Diagonálně
            for (int i = 0; i < 2; i++) {
                int dx = diagonal_dirs[dir_index][i];
                int new_x = old_x + dx;
                if (new_x >= 0 && new_x < width && test_y < height && grid[test_y][new_x] == nullptr) {
                    particle->x = new_x;
                    particle->y = test_y;
                    grid[test_y][new_x] = particle;
                    moved = true;
                    break;
                }
            }

            // Horizontálně
            if (!moved) {
                for (int distance = 1; distance <= 2 && !moved; distance++) {
                    for (int i = 0; i < 2; i++) {
                        int dx = horizontal_dirs[dir_index][i];
                        int new_x = old_x + (dx * distance);

                        bool path_clear = true;
                        for (int step = 1; step <= distance; step++) {
                            int check_x = old_x + (dx * step);
                            if (check_x < 0 || check_x >= width || grid[old_y][check_x] != nullptr) {
                                path_clear = false;
                                break;
                            }
                        }

                        if (path_clear && new_x >= 0 && new_x < width) {
                            particle->x = new_x;
                            grid[old_y][new_x] = particle;
                            moved = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!moved) {
            particle->settled = true;
            particle->velocity_y = 0;
            if (old_y >= 0 && old_y < height && old_x >= 0 && old_x < width) {
                grid[old_y][old_x] = particle;
            }
        }
    }
}

std::set<Particle*> Board::FindConnectedGroup(Particle* start) {
    std::set<Particle*> visited;
    std::deque<Particle*> queue;
    queue.push_back(start);
    visited.insert(start);
    Color target_color = start->color;

    while (!queue.empty()) {
        Particle* current = queue.front();
        queue.pop_front();

        int cx = (int)current->x;
        int cy = (int)current->y;

        int dirs[][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        for (auto& dir : dirs) {
            int nx = cx + dir[0];
            int ny = cy + dir[1];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                grid[ny][nx] != nullptr && visited.find(grid[ny][nx]) == visited.end()) {
                Particle* neighbor = grid[ny][nx];
                if (neighbor->color.r == target_color.r &&
                    neighbor->color.g == target_color.g &&
                    neighbor->color.b == target_color.b) {
                    visited.insert(neighbor);
                    queue.push_back(neighbor);
                }
            }
        }
    }

    return visited;
}

int Board::CheckHorizontalConnections() {
    // Pokud už probíhá animace výbuchu, nespouštět novou
    if (explosion_state != ExplosionState::NONE) {
        return 0;
    }

    int removed_count = 0;

    for (int y = 0; y < height; y++) {
        if (grid[y][0] == nullptr) continue;

        Particle* start_particle = grid[y][0];
        auto connected_group = FindConnectedGroup(start_particle);

        bool reaches_right = false;
        for (auto p : connected_group) {
            if ((int)p->x == width - 1) {
                reaches_right = true;
                break;
            }
        }

        if (reaches_right) {
            // Zahájit pre-explosion animaci (zoom efekt)
            explosion_state = ExplosionState::ZOOMING;
            explosion_timer = 0;
            particles_to_explode = connected_group;
            particle_scale_factors.clear();

            // Inicializovat scale faktory (1.0 = normální velikost)
            for (auto particle : connected_group) {
                particle_scale_factors[particle] = 1.0f;
            }

            // Uložit barvu pro flash efekt
            explosion_flash_color = start_particle->color;

            removed_count = connected_group.size();
            break;
        }
    }

    return removed_count;
}

void Board::UpdatePreExplosionAnimation() {
    if (explosion_state == ExplosionState::NONE) return;

    explosion_timer++;

    if (explosion_state == ExplosionState::ZOOMING) {
        // Fáze přibližování (zoom) - 60 framů (1 sekunda)
        const int ZOOM_DURATION = 60;
        const float MAX_SCALE = 1.5f;

        if (explosion_timer < ZOOM_DURATION) {
            // Smooth ease-out animace s pulzováním
            float progress = (float)explosion_timer / ZOOM_DURATION;
            float eased = 1.0f - powf(1.0f - progress, 3.0f);

            // Přidat pulzující efekt (sin wave)
            float pulse = sinf((float)explosion_timer * 0.3f) * 0.05f;

            for (auto& pair : particle_scale_factors) {
                // Postupné zvětšování s mírným pulzováním
                pair.second = 1.0f + (MAX_SCALE - 1.0f) * eased + pulse;
            }

            // Postupně zesilující třesení
            int shake_intensity = (int)(progress * 10.0f);
            if (explosion_timer % 3 == 0) {
                TriggerShake(shake_intensity);
            }
        } else {
            // Přechod do fáze exploze
            explosion_state = ExplosionState::EXPLODING;
            explosion_timer = 0;

            // SILNÉ třesení při výbuchu
            int shake_intensity = std::min(30, (int)particles_to_explode.size() / 8);
            TriggerShake(shake_intensity);

            // Vytvořit INTENZIVNÍ explosion particles
            int max_explosion_particles = std::min((int)particles_to_explode.size() * 3, 300);
            int sample_step = std::max(1, (int)particles_to_explode.size() / (max_explosion_particles / 3));

            int i = 0;
            for (auto particle : particles_to_explode) {
                if (i % sample_step == 0) {
                    // Více explosion particles na částici
                    std::uniform_int_distribution<> exp_count(3, 6);
                    int num_explosions = exp_count(gen);
                    for (int j = 0; j < num_explosions; j++) {
                        explosion_particles.emplace_back((int)particle->x, (int)particle->y, particle->color);
                    }
                }
                i++;
            }
        }
    } else if (explosion_state == ExplosionState::EXPLODING) {
        // Fáze exploze - odstranit částice po krátkém zpoždění
        if (explosion_timer > 5) {
            // Odstranit částice
            for (auto particle : particles_to_explode) {
                auto it = std::find(particles.begin(), particles.end(), particle);
                if (it != particles.end()) {
                    delete *it;
                    particles.erase(it);
                }
            }

            // Označit zbývající jako neusazené
            for (auto p : particles) {
                p->settled = false;
            }

            // Vyčistit stav
            particles_to_explode.clear();
            particle_scale_factors.clear();
            explosion_state = ExplosionState::NONE;
            grid_dirty = true;
        }
    }
}

void Board::UpdateExplosions() {
    for (auto& e : explosion_particles) {
        e.Update();
    }
    explosion_particles.erase(
        std::remove_if(explosion_particles.begin(), explosion_particles.end(),
                      [](const ExplosionParticle& e) { return !e.IsAlive(); }),
        explosion_particles.end()
    );
}

void Board::AddTrailsForMovingParticles() {
    // Trail particles jsou vypnuté pro lepší výkon
}

void Board::UpdateTrails() {
    for (auto& t : trail_particles) {
        t.Update();
    }
    trail_particles.erase(
        std::remove_if(trail_particles.begin(), trail_particles.end(),
                      [](const TrailParticle& t) { return !t.IsAlive(); }),
        trail_particles.end()
    );
}

void Board::Draw(int offset_x, int offset_y) {
    // Nakreslit pozadí
    DrawTextureRec(background_texture.texture,
                  {0, 0, (float)width * PARTICLE_SIZE, -(float)height * PARTICLE_SIZE},
                  {(float)offset_x, (float)offset_y}, WHITE);

    // Okraj
    DrawRectangleLines(offset_x - 2, offset_y - 2,
                      width * PARTICLE_SIZE + 4, height * PARTICLE_SIZE + 4,
                      Color{80, 80, 120, 255});

    // Částice - kreslí se každá (se scale faktorem pokud jsou ve výbuchu)
    for (auto p : particles) {
        bool is_exploding = false;
        float scale = 1.0f;

        // Aplikovat scale faktor pokud je částice ve výbuchu
        if (particle_scale_factors.find(p) != particle_scale_factors.end()) {
            scale = particle_scale_factors[p];
            is_exploding = true;
        }

        // Enhanced draw pro částice co budou vybuchovat (se zvětšením)
        p->Draw(offset_x, offset_y, is_exploding, scale);
    }

    // Flash efekt při explozi
    if (explosion_state == ExplosionState::EXPLODING && explosion_timer < 10) {
        float flash_alpha = (1.0f - (float)explosion_timer / 10.0f) * 0.3f;
        Color flash = explosion_flash_color;
        flash.a = (unsigned char)(255 * flash_alpha);
        DrawRectangle(offset_x, offset_y, width * PARTICLE_SIZE, height * PARTICLE_SIZE, flash);
    }

    // Exploze - omezené pro výkon
    int explosion_count = 0;
    for (auto& e : explosion_particles) {
        if (explosion_count++ > 300) break;
        e.Draw(offset_x, offset_y);
    }
}
