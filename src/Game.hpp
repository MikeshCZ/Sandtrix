#pragma once

#include "raylib.h"
#include "GameState.hpp"
#include "Board.hpp"
#include "Tetromino.hpp"
#include "Intro.hpp"
#include "Localization.hpp"

/**
 * Hlavní herní třída řídící stav hry, vstupy a vykreslování.
 * Implementuje stavový automat s přechody mezi obrazovkami (menu, hra, pauza, game over).
 * Zodpovídá za hlavní herní smyčku, zpracování vstupů a orchestraci všech herních komponent.
 */
class Game {
public:
    GameState state;                 // Aktuální stav hry (menu, hra, pauza atd.)
    Board* board;                    // Herní deska se systémem částic
    Tetromino* current_tetromino;    // Aktuálně padající tetromino
    Tetromino* next_tetromino;       // Náhled dalšího tetromina
    Intro* intro;                    // Úvodní animace při startu

    int score;                       // Skóre hráče
    bool game_over;                  // Příznak konce hry
    int fall_counter;                // Počítadlo pro automatický pád tetromina
    int current_fall_speed;          // Aktuální rychlost pádu (snižuje se s vyšším skóre)
    bool waiting_for_settlement;     // Čeká na usazení částic před spawnem nového tetromina
    int offset_x, offset_y;          // Posun desky na obrazovce

    int move_counter_left, move_counter_right, move_counter_down;  // Zpoždění pro plynulé pohyby
    int main_menu_selected, settings_menu_selected, pause_menu_selected;  // Vybrané položky v menu

    int active_gamepad;              // ID aktivního gamepadu (-1 pokud není připojen)
    int gamepad_menu_delay;          // Zpoždění pro navigaci v menu gamepadem
    int gamepad_move_delay_left, gamepad_move_delay_right;  // Zpoždění pro pohyb gamepadem

private:
    bool should_exit;  // Příznak požadavku na ukončení aplikace

public:
    /**
     * Konstruktor - inicializuje herní okno a základní komponenty.
     */
    Game();

    /**
     * Destruktor - uvolňuje paměť alokovanou pro herní objekty.
     */
    ~Game();

    /**
     * Inicializuje novou hru (resetuje skóre, vytvoří novou desku).
     */
    void NewGame();

    /**
     * Vytvoří nové tetromino a umístí ho na vrchol desky.
     */
    void SpawnTetromino();

    /**
     * Detekuje a aktualizuje aktivní gamepad.
     */
    void UpdateGamepad();

    /**
     * Zpracovává vstupy z klávesnice (pohyb, rotace, pauza, menu navigace).
     */
    void HandleInput();

    /**
     * Aktualizuje herní logiku (pohyb tetromina, fyzika, detekce výbuchů).
     */
    void Update();

    /**
     * Vykreslí gradientní pozadí.
     * @param top Barva v horní části obrazovky
     * @param bottom Barva ve spodní části obrazovky
     */
    void DrawGradientBackground(Color top, Color bottom);

    /**
     * Vykreslí hlavní menu s možnostmi Start, Settings, Exit.
     */
    void DrawMainMenu();

    /**
     * Vykreslí menu nastavení (zapnutí/vypnutí hudby).
     */
    void DrawSettingsMenu();

    /**
     * Vykreslí pauza menu s možnostmi Resume, Restart, Main Menu.
     */
    void DrawPauseMenu();

    /**
     * Vykreslí herní obrazovku (desku, tetromina, skóre).
     */
    void DrawGame();

    /**
     * Hlavní vykreslovací metoda - volá odpovídající draw funkci podle aktuálního stavu.
     */
    void Draw();

    /**
     * Spustí hlavní herní smyčku.
     */
    void Run();
};
