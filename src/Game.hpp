#pragma once

#include "raylib.h"
#include "GameState.hpp"
#include "Board.hpp"
#include "Tetromino.hpp"
#include "Intro.hpp"

class Game {
public:
    GameState state;
    Board* board;
    Tetromino* current_tetromino;
    Tetromino* next_tetromino;
    int score;
    bool game_over;
    int fall_counter;
    int offset_x, offset_y;

    int move_counter_left;
    int move_counter_right;
    int move_counter_down;

    // Menu proměnné
    int main_menu_selected;
    int settings_menu_selected;
    int pause_menu_selected;

    // Intro
    Intro* intro;

    Game();
    ~Game();

    void NewGame();
    void SpawnTetromino();
    void HandleInput();
    void Update();
    void DrawGradientBackground(Color top, Color bottom);
    void DrawMainMenu();
    void DrawSettingsMenu();
    void DrawPauseMenu();
    void DrawGame();
    void Draw();
    void Run();
};
