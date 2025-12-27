#ifndef LOCALIZATION_HPP
#define LOCALIZATION_HPP

#include <string>
#include <map>

enum class Language {
    ENGLISH,
    CZECH
};

// Text keys used throughout the game
enum class TextKey {
    // Main Menu
    MAIN_MENU_NEW_GAME,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_EXIT,

    // Settings Menu
    SETTINGS_TITLE,
    SETTINGS_DIFFICULTY_INFO,
    SETTINGS_EASY,
    SETTINGS_NORMAL,
    SETTINGS_HARD,
    SETTINGS_MUSIC,
    SETTINGS_FPS,
    SETTINGS_GAMEPAD,
    SETTINGS_LANGUAGE,
    SETTINGS_BACK,
    SETTINGS_ON,
    SETTINGS_OFF,
    SETTINGS_GAMEPAD_CONNECTED,
    SETTINGS_GAMEPAD_NOT_CONNECTED,

    // Pause Menu
    PAUSE_TITLE,
    PAUSE_RESUME,
    PAUSE_MAIN_MENU,
    PAUSE_EXIT,

    // Game UI
    GAME_SCORE,
    GAME_NEXT_PIECE,
    GAME_DIFFICULTY,
    GAME_OVER_TITLE,
    GAME_OVER_SCORE,
    GAME_OVER_ESC,

    // Difficulty Levels
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,

    // FPS Counter
    FPS_COUNTER
};

class Localization {
private:
    Language current_language;
    std::map<TextKey, std::map<Language, std::string>> texts;

    void InitializeTexts();

public:
    Localization();

    // Get translated text for current language
    const char* GetText(TextKey key) const;

    // Change language
    void SetLanguage(Language lang);
    Language GetLanguage() const { return current_language; }

    // Helper to get language name for display
    const char* GetLanguageName(Language lang) const;
    const char* GetCurrentLanguageName() const;
};

// Global localization instance
extern Localization localization;

#endif
