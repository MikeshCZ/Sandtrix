#pragma once

/**
 * Enum pro stavy hry - definuje všechny možné obrazovky/stavy aplikace.
 * Používá se v hlavní herní smyčce pro přepínání mezi různými režimy.
 */
enum GameState {
    INTRO_SCREEN,       // Úvodní Raylib logo animace
    MAIN_MENU,          // Hlavní menu (Start, Settings, Exit)
    SETTINGS,           // Menu nastavení (hudba)
    PLAYING,            // Aktivní herní stav
    PAUSED,             // Pauza (Resume, Restart, Main Menu)
    GAME_OVER_STATE     // Obrazovka game over se skóre
};
