#include "Board.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <deque>
#include <cmath>

Board::Board() : width(BOARD_WIDTH * PARTICLES_PER_BLOCK), height(BOARD_HEIGHT * PARTICLES_PER_BLOCK),
          shake_amount(0), shake_duration(0), dir_index(0),
          grid_dirty(true),
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
    for (auto& row : grid) {
        std::fill(row.begin(), row.end(), nullptr);
    }

    for (auto p : particles) {
        int px = (int)p->x;
        int py = (int)p->y;
        if (py >= 0 && py < height && px >= 0 && px < width) {
            grid[py][px] = p;
        }
    }
    grid_dirty = false;
}

bool Board::AreAllParticlesSettled() {
    for (auto p : particles) {
        if (!p->settled) {
            return false;
        }
    }
    return true;
}

void Board::ApplyGravity() {
    // Pokud je mřížka dirty, rebuild ji
    if (grid_dirty) RebuildGrid();

    // Rychlá kontrola - pokud nejsou žádné neusazené částice, konec
    bool has_unsettled = false;
    for (auto p : particles) {
        if (!p->settled) {
            has_unsettled = true;
            break;
        }
    }
    if (!has_unsettled) return;

    // Kontrola usazených částic - pokud pod nimi není nic, stanou se neusazenými
    for (auto p : particles) {
        int px = (int)p->x;
        int py = (int)p->y;
        if (p->settled && py >= 0 && py + 1 < height && px >= 0 && px < width) {
            if (grid[py + 1][px] == nullptr) {
                p->settled = false;
            }
        }
    }

    // Sesbírat všechny neusazené částice
    std::vector<Particle*> unsettled;
    unsettled.reserve(particles.size() / 4); // Předalokace
    for (auto p : particles) {
        if (!p->settled) {
            unsettled.push_back(p);
        }
    }

    // Limit pro výkon - max 1000 částic na frame
    if (unsettled.size() > 1000) unsettled.resize(1000);

    // Seřadit podle Y (zdola nahoru) pro správné usazování
    std::sort(unsettled.begin(), unsettled.end(),
              [](Particle* a, Particle* b) { return a->y > b->y; });

    // Směry pro diagonální pohyb (alternování pro rovnoměrné rozprostření)
    dir_index = (dir_index + 1) % 2;
    int diagonal_dirs[][2] = {{-1, 1}, {1, -1}};

    // Aplikuj gravitaci na každou neusazenou částici
    for (auto particle : unsettled) {
        int old_x = (int)particle->x;
        int old_y = (int)particle->y;
        bool moved = false;

        // Odstranit částici z původní pozice v mřížce
        if (old_y >= 0 && old_y < height && old_x >= 0 && old_x < width) {
            grid[old_y][old_x] = nullptr;
        }

        // Zvýšit rychlost pádu
        particle->velocity_y++;
        int fall_distance = std::min(particle->velocity_y / 2 + 1, 3);

        // Pokus o pád dolů
        int new_y = old_y;
        for (int step = 0; step < fall_distance; step++) {
            int test_y = old_y + step + 1;
            if (test_y >= height || (test_y >= 0 && grid[test_y][old_x] != nullptr)) {
                break;
            }
            new_y = test_y;
        }

        // Pokud se částice posunula dolů
        if (new_y > old_y) {
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
        }
        // Pokud je na dně desky
        else if (old_y + 1 >= height) {
            particle->settled = true;
            particle->velocity_y = 0;
            particle->y = height - 1;
            grid[(int)particle->y][(int)particle->x] = particle;
            moved = true;
        }
        // Pokus o diagonální pohyb (jako písek)
        else {
            particle->velocity_y = 0;
            int test_y = old_y + 1;

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
        }

        // Pokud se částice nepohnula, usadit ji
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
    // BFS algoritmus pro nalezení všech propojených částic stejné barvy
    std::set<Particle*> visited;
    std::deque<Particle*> queue;
    queue.push_back(start);
    visited.insert(start);
    Color target_color = start->color;

    // Směry pro kontrolu sousedů (nahoru, dolů, vlevo, vpravo)
    constexpr int dirs[][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    while (!queue.empty()) {
        Particle* current = queue.front();
        queue.pop_front();

        int cx = (int)current->x;
        int cy = (int)current->y;

        // Zkontroluj všechny 4 sousedy
        for (auto& dir : dirs) {
            int nx = cx + dir[0];
            int ny = cy + dir[1];

            // Validace pozice a kontrola, jestli už nebyl navštíven
            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                grid[ny][nx] != nullptr && visited.find(grid[ny][nx]) == visited.end()) {
                Particle* neighbor = grid[ny][nx];
                // Porovnání barvy (RGB komponenty)
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
    // Pokud probíhá výbuch, neprovádět další kontroly
    if (explosion_state != ExplosionState::NONE) return 0;

    int removed_count = 0;

    // Projít všechny řádky a hledat skupiny, které dosahují od levé po pravou stranu
    for (int y = 0; y < height; y++) {
        if (grid[y][0] == nullptr) continue;

        // Najít všechny propojené částice stejné barvy od levého okraje
        Particle* start_particle = grid[y][0];
        auto connected_group = FindConnectedGroup(start_particle);

        // Zkontrolovat, jestli skupina dosahuje pravého okraje
        bool reaches_right = false;
        for (auto p : connected_group) {
            if ((int)p->x == width - 1) {
                reaches_right = true;
                break;
            }
        }

        // Pokud skupina spojuje levou a pravou stranu, spustit výbuch
        if (reaches_right) {
            explosion_state = ExplosionState::ZOOMING;
            explosion_timer = 0;
            particles_to_explode = connected_group;
            particle_scale_factors.clear();

            // Inicializovat škálovací faktory pro zoom animaci
            for (auto particle : connected_group) {
                particle_scale_factors[particle] = 1.0f;
            }

            explosion_flash_color = start_particle->color;
            removed_count = connected_group.size();
            break; // Pouze jeden výbuch najednou
        }
    }

    return removed_count;
}

void Board::UpdatePreExplosionAnimation() {
    if (explosion_state == ExplosionState::NONE) return;

    explosion_timer++;

    // FÁZE 1: Zoom animace před výbuchem
    if (explosion_state == ExplosionState::ZOOMING) {
        constexpr int ZOOM_DURATION = 60;
        constexpr float MAX_SCALE = 1.5f;

        if (explosion_timer < ZOOM_DURATION) {
            // Vypočítat progres animace s easingem
            float progress = (float)explosion_timer / ZOOM_DURATION;
            float eased = 1.0f - powf(1.0f - progress, 3.0f); // Cubic ease-out
            float pulse = sinf((float)explosion_timer * 0.3f) * 0.05f; // Pulsace

            // Aplikovat škálování na všechny částice určené k výbuchu
            for (auto& pair : particle_scale_factors) {
                pair.second = 1.0f + (MAX_SCALE - 1.0f) * eased + pulse;
            }

            // Třesení obrazovky stoupající s progressem
            if (explosion_timer % 3 == 0) {
                TriggerShake((int)(progress * 10.0f));
            }
        } else {
            // Přechod do fáze výbuchu
            explosion_state = ExplosionState::EXPLODING;
            explosion_timer = 0;

            // Výbuchový efekt třesení
            int shake_intensity = std::min(30, (int)particles_to_explode.size() / 8);
            TriggerShake(shake_intensity);

            // Vytvořit výbuchové částice (optimalizováno pro výkon)
            int max_explosion_particles = std::min((int)particles_to_explode.size() * 3, 300);
            int sample_step = std::max(1, (int)particles_to_explode.size() / (max_explosion_particles / 3));

            int i = 0;
            for (auto particle : particles_to_explode) {
                if (i % sample_step == 0) {
                    std::uniform_int_distribution<> exp_count(3, 6);
                    int num_explosions = exp_count(gen);
                    for (int j = 0; j < num_explosions; j++) {
                        explosion_particles.emplace_back((int)particle->x, (int)particle->y, particle->color);
                    }
                }
                i++;
            }
        }
    }
    // FÁZE 2: Odstranění částic po výbuchu
    else if (explosion_state == ExplosionState::EXPLODING) {
        if (explosion_timer > 5) {
            // Smazat všechny vybuchlé částice
            for (auto particle : particles_to_explode) {
                auto it = std::find(particles.begin(), particles.end(), particle);
                if (it != particles.end()) {
                    delete *it;
                    particles.erase(it);
                }
            }

            // Simulace otřesu desky - přidáme částicím malé náhodné posunutí
            std::uniform_int_distribution<> shake_dist(-10, 10);
            std::uniform_real_distribution<> vertical_shake(0.0f, 20.0f);

            for (auto p : particles) {
                // Horizontální posunutí (vlevo/vpravo)
                int dx = shake_dist(gen);
                int new_x = (int)p->x + dx;

                // Vertikální "vyhození" nahoru (simulace odrazu od země)
                float vertical_impulse = vertical_shake(gen);
                p->velocity_y = -(int)vertical_impulse; // Záporná rychlost = pohyb nahoru

                // Aplikuj horizontální posunutí pokud je validní
                if (new_x >= 0 && new_x < width) {
                    // Zkontroluj jestli není kolize
                    int py = (int)p->y;
                    if (py >= 0 && py < height && grid[py][new_x] == nullptr) {
                        p->x = new_x;
                    }
                }

                // Všechny částice se stávají neusazenými (gravitace je znovu aplikuje)
                p->settled = false;
            }

            // Cleanup a reset stavu výbuchu
            particles_to_explode.clear();
            particle_scale_factors.clear();
            explosion_state = ExplosionState::NONE;

            RebuildGrid();
        }
    }
}

void Board::UpdateExplosions() {
    // Aktualizovat všechny výbuchové částice
    for (auto& e : explosion_particles) e.Update();

    // Odstranit mrtvé částice (erase-remove idiom)
    explosion_particles.erase(
        std::remove_if(explosion_particles.begin(), explosion_particles.end(),
                      [](const ExplosionParticle& e) { return !e.IsAlive(); }),
        explosion_particles.end()
    );
}

void Board::Draw(int offset_x, int offset_y) {
    DrawTextureRec(background_texture.texture,
                  {0, 0, (float)width * PARTICLE_SIZE, -(float)height * PARTICLE_SIZE},
                  {(float)offset_x, (float)offset_y}, WHITE);

    DrawRectangleLines(offset_x - 2, offset_y - 2,
                      width * PARTICLE_SIZE + 4, height * PARTICLE_SIZE + 4,
                      Color{80, 80, 120, 255});

    for (auto p : particles) {
        bool is_exploding = false;
        float scale = 1.0f;

        if (particle_scale_factors.find(p) != particle_scale_factors.end()) {
            scale = particle_scale_factors[p];
            is_exploding = true;
        }

        p->Draw(offset_x, offset_y, is_exploding, scale);
    }

    if (explosion_state == ExplosionState::EXPLODING && explosion_timer < 10) {
        float flash_alpha = (1.0f - (float)explosion_timer / 10.0f) * 0.3f;
        Color flash = explosion_flash_color;
        flash.a = (unsigned char)(255 * flash_alpha);
        DrawRectangle(offset_x, offset_y, width * PARTICLE_SIZE, height * PARTICLE_SIZE, flash);
    }

    int explosion_count = 0;
    for (auto& e : explosion_particles) {
        if (explosion_count++ > 300) break;
        e.Draw(offset_x, offset_y);
    }
}
