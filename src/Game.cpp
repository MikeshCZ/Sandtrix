#include "Game.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include "Shapes.hpp"
#include <cmath>

Game::Game() : state(INTRO_SCREEN), board(nullptr), current_tetromino(nullptr),
         next_tetromino(nullptr), score(0), game_over(false), fall_counter(0),
         offset_x(50), offset_y(50), move_counter_left(0), move_counter_right(0),
         move_counter_down(0), main_menu_selected(0), settings_menu_selected(0),
         pause_menu_selected(0), intro(nullptr) {
    intro = new Intro("SAND TETRIS", SCREEN_WIDTH, SCREEN_HEIGHT);
}

Game::~Game() {
    if (board) delete board;
    if (current_tetromino) delete current_tetromino;
    if (next_tetromino) delete next_tetromino;
    if (intro) delete intro;
}

void Game::NewGame() {
    if (board) delete board;
    if (current_tetromino) delete current_tetromino;
    if (next_tetromino) delete next_tetromino;

    board = new Board();
    current_tetromino = nullptr;
    next_tetromino = new Tetromino(0, 0);
    score = 0;
    game_over = false;
    fall_counter = 0;
    move_counter_left = 0;
    move_counter_right = 0;
    move_counter_down = 0;

    SpawnTetromino();
    state = PLAYING;
}

void Game::SpawnTetromino() {
    if (current_tetromino) delete current_tetromino;

    current_tetromino = new Tetromino(BOARD_WIDTH / 2 - 2, 0);
    if (next_tetromino) {
        current_tetromino->shape_type = next_tetromino->shape_type;
        current_tetromino->color = next_tetromino->color;
        current_tetromino->rotation = 0;
        current_tetromino->GenerateParticles();

        delete next_tetromino;
        next_tetromino = new Tetromino(0, 0);
    }

    if (board->CheckCollision(current_tetromino->particles)) {
        game_over = true;
    }
}

void Game::HandleInput() {
    if (state == INTRO_SCREEN) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
            state = MAIN_MENU;
        }
    } else if (state == MAIN_MENU) {
        if (IsKeyPressed(KEY_UP)) {
            main_menu_selected = (main_menu_selected - 1 + 3) % 3;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            main_menu_selected = (main_menu_selected + 1) % 3;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (main_menu_selected == 0) NewGame();
            else if (main_menu_selected == 1) state = SETTINGS;
            else CloseWindow();
        }
    } else if (state == SETTINGS) {
        if (IsKeyPressed(KEY_UP)) {
            settings_menu_selected = (settings_menu_selected - 1 + 4) % 4;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            settings_menu_selected = (settings_menu_selected + 1) % 4;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (settings_menu_selected == 3) {
                MUSIC_ENABLED = !MUSIC_ENABLED;
            } else {
                int colors[] = {3, 4, 6};
                NUM_COLORS = colors[settings_menu_selected];
                state = MAIN_MENU;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            state = MAIN_MENU;
        }
    } else if (state == PAUSED) {
        if (IsKeyPressed(KEY_UP)) {
            pause_menu_selected = (pause_menu_selected - 1 + 3) % 3;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            pause_menu_selected = (pause_menu_selected + 1) % 3;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (pause_menu_selected == 0) state = PLAYING;
            else if (pause_menu_selected == 1) state = MAIN_MENU;
            else CloseWindow();
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            state = PLAYING;
        }
    } else if (state == PLAYING) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (game_over) {
                state = MAIN_MENU;
            } else {
                state = PAUSED;
            }
            return;
        }

        if (game_over || !current_tetromino || !current_tetromino->is_active) return;

        if (IsKeyPressed(KEY_UP)) {
            int old_rotation = current_tetromino->rotation;
            current_tetromino->Rotate();
            if (board->CheckCollision(current_tetromino->particles)) {
                current_tetromino->rotation = old_rotation;
                current_tetromino->GenerateParticles();
            }
        }

        bool moved = false;
        if (IsKeyDown(KEY_LEFT) && !moved) {
            if (move_counter_left == 0) {
                current_tetromino->Move(-1, 0);
                if (board->CheckCollision(current_tetromino->particles)) {
                    current_tetromino->Move(1, 0);
                }
                move_counter_left = 1;
                moved = true;
            } else {
                move_counter_left++;
                if (move_counter_left >= MOVE_DELAY) {
                    current_tetromino->Move(-1, 0);
                    if (board->CheckCollision(current_tetromino->particles)) {
                        current_tetromino->Move(1, 0);
                    }
                    move_counter_left = 1;
                    moved = true;
                }
            }
        } else if (!IsKeyDown(KEY_LEFT)) {
            move_counter_left = 0;
        }

        if (IsKeyDown(KEY_RIGHT) && !moved) {
            if (move_counter_right == 0) {
                current_tetromino->Move(1, 0);
                if (board->CheckCollision(current_tetromino->particles)) {
                    current_tetromino->Move(-1, 0);
                }
                move_counter_right = 1;
                moved = true;
            } else {
                move_counter_right++;
                if (move_counter_right >= MOVE_DELAY) {
                    current_tetromino->Move(1, 0);
                    if (board->CheckCollision(current_tetromino->particles)) {
                        current_tetromino->Move(-1, 0);
                    }
                    move_counter_right = 1;
                    moved = true;
                }
            }
        } else if (!IsKeyDown(KEY_RIGHT)) {
            move_counter_right = 0;
        }

        if (IsKeyDown(KEY_DOWN)) {
            if (move_counter_down == 0) {
                fall_counter = FALL_SPEED;
                move_counter_down = 1;
            } else {
                move_counter_down++;
                if (move_counter_down >= 3) {
                    fall_counter = FALL_SPEED;
                    move_counter_down = 1;
                }
            }
        } else {
            move_counter_down = 0;
        }
    }
}

void Game::Update() {
    if (state == INTRO_SCREEN) {
        if (intro) {
            intro->UpdateLogoScreen();
            if (intro->IsIntroFininsh()) {
                state = MAIN_MENU;
            }
        }
        return;
    }

    if (state != PLAYING || game_over) return;

    if (current_tetromino && current_tetromino->is_active) {
        fall_counter++;

        if (fall_counter >= FALL_SPEED) {
            fall_counter = 0;
            current_tetromino->Move(0, 1);

            if (board->CheckCollision(current_tetromino->particles)) {
                current_tetromino->Move(0, -1);

                for (auto& p : current_tetromino->particles) p.settled = false;

                board->AddParticles(current_tetromino->particles);
                board->grid_dirty = true;
                SpawnTetromino();
            }
        }
    }

    board->ApplyGravity();
    board->UpdatePreExplosionAnimation();
    board->UpdateExplosions();
    board->UpdateShake();

    int removed = board->CheckHorizontalConnections();
    if (removed > 0) {
        score += removed;
        board->grid_dirty = true;
    }
}

void Game::DrawGradientBackground(Color top, Color bottom) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float blend = (float)y / SCREEN_HEIGHT;
        Color c = BlendColors(top, bottom, blend);
        DrawLine(0, y, SCREEN_WIDTH, y, c);
    }
}

void Game::DrawMainMenu() {
    DrawGradientBackground(BG_COLOR_TOP, BG_COLOR_BOTTOM);

    DrawText("SAND TETRIS", SCREEN_WIDTH / 2 - MeasureText("SAND TETRIS", 72) / 2, 150, 72, WHITE);

    float time = GetTime();
    for (int i = 0; i < 20; i++) {
        float x = 100 + i * 35 + sin(time + i * 0.5f) * 20;
        float y = 250 + cos(time * 0.8f + i * 0.3f) * 30;
        float size = 3 + sin(time * 2 + i) * 2;
        unsigned char alpha = (unsigned char)(100 + sin(time * 3 + i * 0.7f) * 50);
        Color particle_color = ColorWithAlpha(ALL_COLORS[i % 6], alpha);
        DrawCircle((int)x, (int)y, size, particle_color);
    }

    const char* items[] = {"Nová hra", "Nastavení", "Konec"};
    int y_start = 350;
    int y_spacing = 80;

    for (int i = 0; i < 3; i++) {
        Color color = (i == main_menu_selected) ? Color{255, 100, 0, 255} : WHITE;
        int text_width = MeasureText(items[i], 48);
        DrawText(items[i], SCREEN_WIDTH / 2 - text_width / 2, y_start + i * y_spacing, 48, color);

        if (i == main_menu_selected) {
            DrawText(">", SCREEN_WIDTH / 2 - text_width / 2 - 40, y_start + i * y_spacing, 48, color);
        }
    }
}

void Game::DrawSettingsMenu() {
    DrawGradientBackground(BG_COLOR_TOP, BG_COLOR_BOTTOM);

    DrawText("NASTAVENÍ", SCREEN_WIDTH / 2 - MeasureText("NASTAVENÍ", 72) / 2, 150, 72, WHITE);

    const char* info = "Obtížnost určuje počet barev:";
    DrawText(info, SCREEN_WIDTH / 2 - MeasureText(info, 28) / 2, 250, 28, Color{200, 200, 220, 255});

    const char* items[] = {"Lehká (3 barvy)", "Normální (4 barvy)", "Těžká (6 barev)",
                            MUSIC_ENABLED ? "Hudba: ZAPNUTO" : "Hudba: VYPNUTO"};
    int y_start = 330;
    int y_spacing = 70;

    for (int i = 0; i < 4; i++) {
        Color color = (i == settings_menu_selected) ? Color{255, 100, 0, 255} : WHITE;
        int text_width = MeasureText(items[i], 48);
        DrawText(items[i], SCREEN_WIDTH / 2 - text_width / 2, y_start + i * y_spacing, 48, color);

        if (i == settings_menu_selected) {
            DrawText(">", SCREEN_WIDTH / 2 - text_width / 2 - 40, y_start + i * y_spacing, 48, color);
        }
    }

    const char* esc = "ESC = Zpět";
    DrawText(esc, SCREEN_WIDTH / 2 - MeasureText(esc, 28) / 2, 750, 28, Color{150, 150, 170, 255});
}

void Game::DrawPauseMenu() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorWithAlpha(BLACK, 200));

    DrawText("PAUZA", SCREEN_WIDTH / 2 - MeasureText("PAUZA", 72) / 2, 150, 72, WHITE);

    const char* items[] = {"Pokračovat", "Hlavní menu", "Konec"};
    int y_start = 350;
    int y_spacing = 80;

    for (int i = 0; i < 3; i++) {
        Color color = (i == pause_menu_selected) ? Color{255, 100, 0, 255} : WHITE;
        int text_width = MeasureText(items[i], 48);
        DrawText(items[i], SCREEN_WIDTH / 2 - text_width / 2, y_start + i * y_spacing, 48, color);

        if (i == pause_menu_selected) {
            DrawText(">", SCREEN_WIDTH / 2 - text_width / 2 - 40, y_start + i * y_spacing, 48, color);
        }
    }
}

void Game::DrawGame() {
    DrawGradientBackground(BG_COLOR_TOP, BG_COLOR_BOTTOM);

    Vector2 shake = board ? board->GetShakeOffset() : Vector2{0, 0};
    int shake_offset_x = offset_x + (int)shake.x;
    int shake_offset_y = offset_y + (int)shake.y;

    if (board) {
        board->Draw(shake_offset_x, shake_offset_y);
    }

    if (current_tetromino && current_tetromino->is_active) {
        current_tetromino->Draw(shake_offset_x, shake_offset_y);
    }

    int panel_x = 520;
    int panel_y = 50;
    int panel_width = 250;
    int panel_height = 500;

    DrawRectangle(panel_x, panel_y, panel_width, panel_height, ColorWithAlpha(Color{30, 30, 45, 255}, 200));
    DrawRectangleLines(panel_x, panel_y, panel_width, panel_height, Color{100, 100, 150, 255});

    DrawText("SKÓRE", panel_x + 20, panel_y + 20, 28, Color{150, 150, 200, 255});
    DrawText(TextFormat("%d", score), panel_x + 20, panel_y + 50, 48, WHITE);

    DrawText("DALŠÍ KOSTKA", panel_x + 20, panel_y + 120, 28, Color{150, 150, 200, 255});

    if (next_tetromino) {
        int preview_box_x = panel_x + 50;
        int preview_box_y = panel_y + 155;
        int preview_box_size = 160;

        DrawRectangle(preview_box_x, preview_box_y, preview_box_size, preview_box_size,
                     ColorWithAlpha(Color{20, 20, 30, 255}, 150));
        DrawRectangleLines(preview_box_x, preview_box_y, preview_box_size, preview_box_size,
                         Color{80, 80, 120, 255});

        auto shape = GetShape(next_tetromino->shape_type, 0);
        if (!shape.empty()) {
            int min_x = 100, max_x = 0, min_y = 100, max_y = 0;
            for (auto& block : shape) {
                if (block.x < min_x) min_x = (int)block.x;
                if (block.x > max_x) max_x = (int)block.x;
                if (block.y < min_y) min_y = (int)block.y;
                if (block.y > max_y) max_y = (int)block.y;
            }

            int shape_width = (max_x - min_x + 1) * CELL_SIZE;
            int shape_height = (max_y - min_y + 1) * CELL_SIZE;
            int center_offset_x = preview_box_x + (preview_box_size - shape_width) / 2 - min_x * CELL_SIZE;
            int center_offset_y = preview_box_y + (preview_box_size - shape_height) / 2 - min_y * CELL_SIZE;

            for (auto& block : shape) {
                for (int px = 0; px < PARTICLES_PER_BLOCK; px++) {
                    for (int py = 0; py < PARTICLES_PER_BLOCK; py++) {
                        int x_pos = center_offset_x + (int)block.x * CELL_SIZE + px * PARTICLE_SIZE;
                        int y_pos = center_offset_y + (int)block.y * CELL_SIZE + py * PARTICLE_SIZE;

                        DrawRectangle(x_pos, y_pos, PARTICLE_SIZE, PARTICLE_SIZE, next_tetromino->color);

                        Color highlight = BrightenColor(next_tetromino->color, 1.3f);
                        DrawLine(x_pos, y_pos, x_pos + PARTICLE_SIZE - 1, y_pos, highlight);
                        DrawLine(x_pos, y_pos, x_pos, y_pos + PARTICLE_SIZE - 1, highlight);
                    }
                }
            }
        }
    }

    DrawText("OBTÍŽNOST", panel_x + 20, panel_y + 335, 28, Color{150, 150, 200, 255});

    const char* diff_names[] = {"Lehká", "Normální", "Těžká"};
    Color diff_colors[] = {{100, 255, 100, 255}, {255, 200, 100, 255}, {255, 100, 100, 255}};
    int diff_idx = (NUM_COLORS == 3) ? 0 : (NUM_COLORS == 4) ? 1 : 2;
    DrawText(diff_names[diff_idx], panel_x + 20, panel_y + 365, 36, diff_colors[diff_idx]);

    if (game_over) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorWithAlpha(BLACK, 180));

        const char* go_text = "GAME OVER";
        int go_width = MeasureText(go_text, 96);
        DrawText(go_text, SCREEN_WIDTH / 2 - go_width / 2, SCREEN_HEIGHT / 2 - 50, 96, Color{255, 80, 80, 255});

        const char* score_text = TextFormat("Skóre: %d", score);
        int score_width = MeasureText(score_text, 48);
        DrawText(score_text, SCREEN_WIDTH / 2 - score_width / 2, SCREEN_HEIGHT / 2 + 30, 48, WHITE);

        const char* esc_text = "ESC = Menu";
        int esc_width = MeasureText(esc_text, 36);
        DrawText(esc_text, SCREEN_WIDTH / 2 - esc_width / 2, SCREEN_HEIGHT / 2 + 100, 36, Color{200, 200, 200, 255});
    }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (state) {
        case INTRO_SCREEN:
            ClearBackground(RAYWHITE);
            if (intro) {
                intro->DrawLogoScreen();
            }
            break;
        case MAIN_MENU:
            DrawMainMenu();
            break;
        case SETTINGS:
            DrawSettingsMenu();
            break;
        case PLAYING:
            DrawGame();
            break;
        case PAUSED:
            DrawGame();
            DrawPauseMenu();
            break;
        default:
            break;
    }

    if (state != INTRO_SCREEN) {
        int particle_count = (board && state == PLAYING) ? board->particles.size() : 0;
        DrawText(TextFormat("FPS: %d | Particles: %d", GetFPS(), particle_count), 10, 10, 20, Color{0, 255, 0, 255});
    }

    EndDrawing();
}

void Game::Run() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand Tetris");
    SetTargetFPS(FPS);

    InitAudioDevice();
    Music music = LoadMusicStream("assets/music.ogg");
    SetMusicVolume(music, 1.0f);

    if (MUSIC_ENABLED) PlayMusicStream(music);

    while (!WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();

        if (MUSIC_ENABLED && !IsMusicStreamPlaying(music)) {
            PlayMusicStream(music);
        } else if (!MUSIC_ENABLED && IsMusicStreamPlaying(music)) {
            StopMusicStream(music);
        }

        if (IsMusicStreamPlaying(music)) UpdateMusicStream(music);
    }

    CloseWindow();
}
