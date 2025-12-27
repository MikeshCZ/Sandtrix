#include "Game.hpp"
#include "Constants.hpp"
#include "Utils.hpp"
#include "Shapes.hpp"
#include <cmath>

// Konstruktor - inicializace hry
Game::Game() : state(INTRO_SCREEN), board(nullptr), current_tetromino(nullptr),
         next_tetromino(nullptr), score(0), game_over(false), fall_counter(0),
         offset_x(50), offset_y(50), move_counter_left(0), move_counter_right(0),
         move_counter_down(0), main_menu_selected(0), settings_menu_selected(0),
         pause_menu_selected(0), intro(nullptr), should_exit(false),
         active_gamepad(-1), gamepad_menu_delay(0),
         gamepad_move_delay_left(0), gamepad_move_delay_right(0) {
    // Vytvořit úvodní animaci
    intro = new Intro(GAME_NAME.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT);

    // Detekce připojeného gamepadu při startu (max 4 gamepady)
    for (int i = 0; i < 4; i++) {
        if (IsGamepadAvailable(i)) {
            active_gamepad = i;
            break;
        }
    }
}

// Destruktor - cleanup všech alokovaných objektů
Game::~Game() {
    if (board) delete board;
    if (current_tetromino) delete current_tetromino;
    if (next_tetromino) delete next_tetromino;
    if (intro) delete intro;
}

// Spustit novou hru - reset všech herních hodnot
void Game::NewGame() {
    // Smazat staré objekty pokud existují
    if (board) delete board;
    if (current_tetromino) delete current_tetromino;
    if (next_tetromino) delete next_tetromino;

    // Vytvořit nové herní objekty
    board = new Board();
    current_tetromino = nullptr;
    next_tetromino = new Tetromino(0, 0);

    // Reset herního stavu
    score = 0;
    game_over = false;
    fall_counter = 0;
    move_counter_left = 0;
    move_counter_right = 0;
    move_counter_down = 0;

    // Spawn prvního tetromina a přejít do herního stavu
    SpawnTetromino();
    state = PLAYING;
}

// Vytvořit nové tetromino na vrcholu desky
void Game::SpawnTetromino() {
    if (current_tetromino) delete current_tetromino;

    // Vytvořit tetromino na středu desky (x), na vrcholu (y = 0)
    current_tetromino = new Tetromino(BOARD_WIDTH / 2 - 2, 0);

    // Zkopírovat tvar a barvu z next_tetromino (preview)
    if (next_tetromino) {
        current_tetromino->shape_type = next_tetromino->shape_type;
        current_tetromino->color = next_tetromino->color;
        current_tetromino->rotation = 0;
        current_tetromino->GenerateParticles();

        // Vytvořit nové next_tetromino
        delete next_tetromino;
        next_tetromino = new Tetromino(0, 0);
    }

    // Kontrola game over - pokud nové tetromino koliduje hned při spawnu
    if (board->CheckCollision(current_tetromino->particles)) {
        game_over = true;
    }
}

void Game::UpdateGamepad() {
    // Zkontroluj, jestli je aktivní gamepad stále připojen
    if (active_gamepad >= 0 && !IsGamepadAvailable(active_gamepad)) {
        active_gamepad = -1;
    }

    // Pokud není aktivní gamepad, zkus najít připojený
    if (active_gamepad < 0) {
        for (int i = 0; i < 4; i++) {
            if (IsGamepadAvailable(i)) {
                active_gamepad = i;
                break;
            }
        }
    }
}

void Game::HandleInput() {
    if (state == INTRO_SCREEN) {
        bool skip = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE);

        // Gamepad: A button pro skip
        if (active_gamepad >= 0) {
            skip = skip || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A button
        }

        if (skip) {
            state = MAIN_MENU;
        }
    } else if (state == MAIN_MENU) {
        bool up = IsKeyPressed(KEY_UP);
        bool down = IsKeyPressed(KEY_DOWN);
        bool enter = IsKeyPressed(KEY_ENTER);

        // Gamepad: levá páčka nebo D-pad pro navigaci
        if (active_gamepad >= 0) {
            float axis_y = GetGamepadAxisMovement(active_gamepad, GAMEPAD_AXIS_LEFT_Y);
            bool dpad_up = IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
            bool dpad_down = IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN);

            // Delay pro páčku aby se necyklovala moc rychle
            if (gamepad_menu_delay == 0) {
                if (axis_y < -0.5f || dpad_up) {
                    up = true;
                    gamepad_menu_delay = 15;
                } else if (axis_y > 0.5f || dpad_down) {
                    down = true;
                    gamepad_menu_delay = 15;
                }
            } else {
                gamepad_menu_delay--;
            }

            enter = enter || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A button
        }

        if (up) {
            main_menu_selected = (main_menu_selected - 1 + 3) % 3;
        }
        if (down) {
            main_menu_selected = (main_menu_selected + 1) % 3;
        }
        if (enter) {
            if (main_menu_selected == 0) NewGame();
            else if (main_menu_selected == 1) state = SETTINGS;
            else should_exit = true;
        }
    } else if (state == SETTINGS) {
        bool up = IsKeyPressed(KEY_UP);
        bool down = IsKeyPressed(KEY_DOWN);
        bool enter = IsKeyPressed(KEY_ENTER);
        bool escape = IsKeyPressed(KEY_ESCAPE);

        // Gamepad: levá páčka nebo D-pad pro navigaci
        if (active_gamepad >= 0) {
            float axis_y = GetGamepadAxisMovement(active_gamepad, GAMEPAD_AXIS_LEFT_Y);
            bool dpad_up = IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
            bool dpad_down = IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN);

            if (gamepad_menu_delay == 0) {
                if (axis_y < -0.5f || dpad_up) {
                    up = true;
                    gamepad_menu_delay = 15;
                } else if (axis_y > 0.5f || dpad_down) {
                    down = true;
                    gamepad_menu_delay = 15;
                }
            } else {
                gamepad_menu_delay--;
            }

            enter = enter || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A button
            escape = escape || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT); // B button
        }

        if (up) {
            settings_menu_selected = (settings_menu_selected - 1 + 7) % 7;
        }
        if (down) {
            settings_menu_selected = (settings_menu_selected + 1) % 7;
        }
        if (enter) {
            if (settings_menu_selected == 3) {
                MUSIC_ENABLED = !MUSIC_ENABLED;
            } else if (settings_menu_selected == 4) {
                FPS_ENABLED = !FPS_ENABLED;
            } else if (settings_menu_selected == 5) {
                // Výběr gamepadu - přepne na další dostupný
                int next_gamepad = (active_gamepad + 1) % 4;
                for (int i = 0; i < 4; i++) {
                    if (IsGamepadAvailable(next_gamepad)) {
                        active_gamepad = next_gamepad;
                        break;
                    }
                    next_gamepad = (next_gamepad + 1) % 4;
                }
            } else if (settings_menu_selected == 6) {
                // Přepnutí jazyka
                Language current = localization.GetLanguage();
                if (current == Language::ENGLISH) {
                    localization.SetLanguage(Language::CZECH);
                } else {
                    localization.SetLanguage(Language::ENGLISH);
                }
            } else {
                int colors[] = {3, 4, 6};
                NUM_COLORS = colors[settings_menu_selected];
                state = MAIN_MENU;
            }
        }
        if (escape) {
            state = MAIN_MENU;
        }
    } else if (state == PAUSED) {
        bool up = IsKeyPressed(KEY_UP);
        bool down = IsKeyPressed(KEY_DOWN);
        bool enter = IsKeyPressed(KEY_ENTER);
        bool escape = IsKeyPressed(KEY_ESCAPE);

        // Gamepad: levá páčka nebo D-pad pro navigaci
        if (active_gamepad >= 0) {
            float axis_y = GetGamepadAxisMovement(active_gamepad, GAMEPAD_AXIS_LEFT_Y);
            bool dpad_up = IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
            bool dpad_down = IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN);

            if (gamepad_menu_delay == 0) {
                if (axis_y < -0.5f || dpad_up) {
                    up = true;
                    gamepad_menu_delay = 15;
                } else if (axis_y > 0.5f || dpad_down) {
                    down = true;
                    gamepad_menu_delay = 15;
                }
            } else {
                gamepad_menu_delay--;
            }

            enter = enter || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN); // A button
            escape = escape || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT); // B button
        }

        if (up) {
            pause_menu_selected = (pause_menu_selected - 1 + 3) % 3;
        }
        if (down) {
            pause_menu_selected = (pause_menu_selected + 1) % 3;
        }
        if (enter) {
            if (pause_menu_selected == 0) state = PLAYING;
            else if (pause_menu_selected == 1) state = MAIN_MENU;
            else should_exit = true;
        }
        if (escape) {
            state = PLAYING;
        }
    } else if (state == PLAYING) {
        bool escape = IsKeyPressed(KEY_ESCAPE);

        // Gamepad: START button = pauza/ESC
        if (active_gamepad >= 0) {
            escape = escape || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_MIDDLE_RIGHT); // START button
        }

        if (escape) {
            if (game_over) {
                state = MAIN_MENU;
            } else {
                state = PAUSED;
            }
            return;
        }

        if (game_over || !current_tetromino || !current_tetromino->is_active) return;

        bool rotate = IsKeyPressed(KEY_UP);

        // Gamepad: B button = rotace
        if (active_gamepad >= 0) {
            rotate = rotate || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsGamepadButtonPressed(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_UP);
        }

        if (rotate) {
            int old_rotation = current_tetromino->rotation;
            current_tetromino->Rotate();
            if (board->CheckCollision(current_tetromino->particles)) {
                current_tetromino->rotation = old_rotation;
                current_tetromino->GenerateParticles();
            }
        }

        bool moved = false;
        bool left_pressed = IsKeyDown(KEY_LEFT);
        bool right_pressed = IsKeyDown(KEY_RIGHT);

        // Gamepad: levá páčka nebo D-pad doleva/doprava
        if (active_gamepad >= 0) {
            float axis_x = GetGamepadAxisMovement(active_gamepad, GAMEPAD_AXIS_LEFT_X);
            bool dpad_left = IsGamepadButtonDown(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
            bool dpad_right = IsGamepadButtonDown(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);

            left_pressed = left_pressed || (axis_x < -0.5f) || dpad_left;
            right_pressed = right_pressed || (axis_x > 0.5f) || dpad_right;
        }

        if (left_pressed && !moved) {
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
        } else if (!left_pressed) {
            move_counter_left = 0;
        }

        if (right_pressed && !moved) {
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
        } else if (!right_pressed) {
            move_counter_right = 0;
        }

        bool down_pressed = IsKeyDown(KEY_DOWN);

        // Gamepad: A button = rychlý pád
        if (active_gamepad >= 0) {
            down_pressed = down_pressed || IsGamepadButtonDown(active_gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonDown(active_gamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
        }

        if (down_pressed) {
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

// Aktualizace herní logiky
void Game::Update() {
    // Update úvodní animace
    if (state == INTRO_SCREEN) {
        if (intro) {
            intro->UpdateLogoScreen();
            if (intro->IsIntroFininsh()) {
                state = MAIN_MENU;
            }
        }
        return;
    }

    // Update pouze když je hra aktivní
    if (state != PLAYING || game_over) return;

    // Update padajícího tetromina
    if (current_tetromino && current_tetromino->is_active) {
        fall_counter++;

        // Automatický pád tetromina podle FALL_SPEED
        if (fall_counter >= FALL_SPEED) {
            fall_counter = 0;
            current_tetromino->Move(0, 1);

            // Pokud tetromino narazilo, umístit ho na desku
            if (board->CheckCollision(current_tetromino->particles)) {
                current_tetromino->Move(0, -1);

                // Všechny částice budou podléhat gravitaci
                for (auto& p : current_tetromino->particles) p.settled = false;

                // Přidat částice na desku a spawn nového tetromina
                board->AddParticles(current_tetromino->particles);
                board->grid_dirty = true;
                SpawnTetromino();
            }
        }
    }

    // Update fyziky a výbuchů
    board->ApplyGravity();
    board->UpdatePreExplosionAnimation();
    board->UpdateExplosions();
    board->UpdateShake();

    // Kontrola propojených částic a výpočet skóre
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

    DrawText(GAME_NAME.c_str(), SCREEN_WIDTH / 2 - MeasureText(GAME_NAME.c_str(), 72) / 2, 150, 72, WHITE);

    float time = GetTime();
    for (int i = 0; i < 20; i++) {
        float x = 100 + i * 35 + sin(time + i * 0.5f) * 20;
        float y = 250 + cos(time * 0.8f + i * 0.3f) * 30;
        float size = 3 + sin(time * 2 + i) * 2;
        unsigned char alpha = (unsigned char)(100 + sin(time * 3 + i * 0.7f) * 50);
        Color particle_color = ColorWithAlpha(ALL_COLORS[i % 6], alpha);
        DrawCircle((int)x, (int)y, size, particle_color);
    }

    const char* items[] = {
        localization.GetText(TextKey::MAIN_MENU_NEW_GAME),
        localization.GetText(TextKey::MAIN_MENU_SETTINGS),
        localization.GetText(TextKey::MAIN_MENU_EXIT)
    };
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

    const char* title = localization.GetText(TextKey::SETTINGS_TITLE);
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 72) / 2, 150, 72, WHITE);

    const char* info = localization.GetText(TextKey::SETTINGS_DIFFICULTY_INFO);
    DrawText(info, SCREEN_WIDTH / 2 - MeasureText(info, 28) / 2, 250, 28, Color{200, 200, 220, 255});

    // Gamepad text
    char gamepad_text[128];
    if (active_gamepad >= 0 && IsGamepadAvailable(active_gamepad)) {
        const char* gamepad_name = GetGamepadName(active_gamepad);
        snprintf(gamepad_text, sizeof(gamepad_text), "%s: %s",
                 localization.GetText(TextKey::SETTINGS_GAMEPAD),
                 gamepad_name ? gamepad_name : localization.GetText(TextKey::SETTINGS_GAMEPAD_CONNECTED));
    } else {
        snprintf(gamepad_text, sizeof(gamepad_text), "%s: %s",
                 localization.GetText(TextKey::SETTINGS_GAMEPAD),
                 localization.GetText(TextKey::SETTINGS_GAMEPAD_NOT_CONNECTED));
    }

    // Music text
    char music_text[64];
    snprintf(music_text, sizeof(music_text), "%s: %s",
             localization.GetText(TextKey::SETTINGS_MUSIC),
             MUSIC_ENABLED ? localization.GetText(TextKey::SETTINGS_ON) : localization.GetText(TextKey::SETTINGS_OFF));

    // FPS text
    char fps_text[64];
    snprintf(fps_text, sizeof(fps_text), "%s: %s",
             localization.GetText(TextKey::SETTINGS_FPS),
             FPS_ENABLED ? localization.GetText(TextKey::SETTINGS_ON) : localization.GetText(TextKey::SETTINGS_OFF));

    // Language text
    char language_text[64];
    snprintf(language_text, sizeof(language_text), "%s: %s",
             localization.GetText(TextKey::SETTINGS_LANGUAGE),
             localization.GetCurrentLanguageName());

    const char* items[] = {
        localization.GetText(TextKey::SETTINGS_EASY),
        localization.GetText(TextKey::SETTINGS_NORMAL),
        localization.GetText(TextKey::SETTINGS_HARD),
        music_text,
        fps_text,
        gamepad_text,
        language_text
    };
    int y_start = 290;
    int y_spacing = 60;

    for (int i = 0; i < 7; i++) {
        Color color = (i == settings_menu_selected) ? Color{255, 100, 0, 255} : WHITE;
        int text_width = MeasureText(items[i], 48);
        DrawText(items[i], SCREEN_WIDTH / 2 - text_width / 2, y_start + i * y_spacing, 48, color);

        if (i == settings_menu_selected) {
            DrawText(">", SCREEN_WIDTH / 2 - text_width / 2 - 40, y_start + i * y_spacing, 48, color);
        }
    }

    const char* esc = localization.GetText(TextKey::SETTINGS_BACK);
    DrawText(esc, SCREEN_WIDTH / 2 - MeasureText(esc, 28) / 2, 750, 28, Color{150, 150, 170, 255});
}

void Game::DrawPauseMenu() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorWithAlpha(BLACK, 200));

    const char* title = localization.GetText(TextKey::PAUSE_TITLE);
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 72) / 2, 150, 72, WHITE);

    const char* items[] = {
        localization.GetText(TextKey::PAUSE_RESUME),
        localization.GetText(TextKey::PAUSE_MAIN_MENU),
        localization.GetText(TextKey::PAUSE_EXIT)
    };
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

    DrawText(localization.GetText(TextKey::GAME_SCORE), panel_x + 20, panel_y + 20, 28, Color{150, 150, 200, 255});
    DrawText(TextFormat("%d", score), panel_x + 20, panel_y + 50, 48, WHITE);

    DrawText(localization.GetText(TextKey::GAME_NEXT_PIECE), panel_x + 20, panel_y + 120, 28, Color{150, 150, 200, 255});

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

    DrawText(localization.GetText(TextKey::GAME_DIFFICULTY), panel_x + 20, panel_y + 335, 28, Color{150, 150, 200, 255});

    Color diff_colors[] = {{100, 255, 100, 255}, {255, 200, 100, 255}, {255, 100, 100, 255}};
    int diff_idx = (NUM_COLORS == 3) ? 0 : (NUM_COLORS == 4) ? 1 : 2;
    const char* diff_names[] = {
        localization.GetText(TextKey::DIFFICULTY_EASY),
        localization.GetText(TextKey::DIFFICULTY_NORMAL),
        localization.GetText(TextKey::DIFFICULTY_HARD)
    };
    DrawText(diff_names[diff_idx], panel_x + 20, panel_y + 365, 36, diff_colors[diff_idx]);

    if (game_over) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorWithAlpha(BLACK, 180));

        const char* go_text = localization.GetText(TextKey::GAME_OVER_TITLE);
        int go_width = MeasureText(go_text, 96);
        DrawText(go_text, SCREEN_WIDTH / 2 - go_width / 2, SCREEN_HEIGHT / 2 - 50, 96, Color{255, 80, 80, 255});

        const char* score_text = TextFormat("%s: %d", localization.GetText(TextKey::GAME_OVER_SCORE), score);
        int score_width = MeasureText(score_text, 48);
        DrawText(score_text, SCREEN_WIDTH / 2 - score_width / 2, SCREEN_HEIGHT / 2 + 30, 48, WHITE);

        const char* esc_text = localization.GetText(TextKey::GAME_OVER_ESC);
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

    if (state != INTRO_SCREEN && FPS_ENABLED) {
        int particle_count = (board && state == PLAYING) ? board->particles.size() : 0;
        DrawText(TextFormat("FPS: %d | Particles: %d", GetFPS(), particle_count), 10, 10, 20, Color{0, 255, 0, 255});
    }

    EndDrawing();
}

void Game::Run() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME.c_str());
    SetExitKey(KEY_NULL);
    SetTargetFPS(FPS);

    InitAudioDevice();
    Music music = LoadMusicStream("assets/music.ogg");
    SetMusicVolume(music, 1.0f);

    if (MUSIC_ENABLED) PlayMusicStream(music);

    while (!WindowShouldClose() && !should_exit) {
        UpdateGamepad();
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

    // Cleanup před zavřením okna
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
}
