#include "Localization.hpp"

// Global instance
Localization localization;

Localization::Localization() : current_language(Language::ENGLISH) {
    InitializeTexts();
}

void Localization::InitializeTexts() {
    // Main Menu
    texts[TextKey::MAIN_MENU_NEW_GAME] = {
        {Language::ENGLISH, "New Game"},
        {Language::CZECH, "Nová hra"}
    };
    texts[TextKey::MAIN_MENU_SETTINGS] = {
        {Language::ENGLISH, "Settings"},
        {Language::CZECH, "Nastavení"}
    };
    texts[TextKey::MAIN_MENU_EXIT] = {
        {Language::ENGLISH, "Exit"},
        {Language::CZECH, "Konec"}
    };

    // Settings Menu
    texts[TextKey::SETTINGS_TITLE] = {
        {Language::ENGLISH, "SETTINGS"},
        {Language::CZECH, "NASTAVENÍ"}
    };
    texts[TextKey::SETTINGS_DIFFICULTY_INFO] = {
        {Language::ENGLISH, "Difficulty determines number of colors:"},
        {Language::CZECH, "Obtížnost určuje počet barev:"}
    };
    texts[TextKey::SETTINGS_EASY] = {
        {Language::ENGLISH, "Easy (3 colors)"},
        {Language::CZECH, "Lehká (3 barvy)"}
    };
    texts[TextKey::SETTINGS_NORMAL] = {
        {Language::ENGLISH, "Normal (4 colors)"},
        {Language::CZECH, "Normální (4 barvy)"}
    };
    texts[TextKey::SETTINGS_HARD] = {
        {Language::ENGLISH, "Hard (6 colors)"},
        {Language::CZECH, "Těžká (6 barev)"}
    };
    texts[TextKey::SETTINGS_MUSIC] = {
        {Language::ENGLISH, "Music"},
        {Language::CZECH, "Hudba"}
    };
    texts[TextKey::SETTINGS_FPS] = {
        {Language::ENGLISH, "FPS"},
        {Language::CZECH, "FPS"}
    };
    texts[TextKey::SETTINGS_GAMEPAD] = {
        {Language::ENGLISH, "Gamepad"},
        {Language::CZECH, "Gamepad"}
    };
    texts[TextKey::SETTINGS_LANGUAGE] = {
        {Language::ENGLISH, "Language"},
        {Language::CZECH, "Jazyk"}
    };
    texts[TextKey::SETTINGS_BACK] = {
        {Language::ENGLISH, "ESC = Back"},
        {Language::CZECH, "ESC = Zpět"}
    };
    texts[TextKey::SETTINGS_ON] = {
        {Language::ENGLISH, "ON"},
        {Language::CZECH, "ZAPNUTO"}
    };
    texts[TextKey::SETTINGS_OFF] = {
        {Language::ENGLISH, "OFF"},
        {Language::CZECH, "VYPNUTO"}
    };
    texts[TextKey::SETTINGS_GAMEPAD_CONNECTED] = {
        {Language::ENGLISH, "Connected"},
        {Language::CZECH, "Připojen"}
    };
    texts[TextKey::SETTINGS_GAMEPAD_NOT_CONNECTED] = {
        {Language::ENGLISH, "Not connected"},
        {Language::CZECH, "Není připojen"}
    };

    // Pause Menu
    texts[TextKey::PAUSE_TITLE] = {
        {Language::ENGLISH, "PAUSED"},
        {Language::CZECH, "PAUZA"}
    };
    texts[TextKey::PAUSE_RESUME] = {
        {Language::ENGLISH, "Resume"},
        {Language::CZECH, "Pokračovat"}
    };
    texts[TextKey::PAUSE_MAIN_MENU] = {
        {Language::ENGLISH, "Main Menu"},
        {Language::CZECH, "Hlavní menu"}
    };
    texts[TextKey::PAUSE_EXIT] = {
        {Language::ENGLISH, "Exit"},
        {Language::CZECH, "Konec"}
    };

    // Game UI
    texts[TextKey::GAME_SCORE] = {
        {Language::ENGLISH, "SCORE"},
        {Language::CZECH, "SKÓRE"}
    };
    texts[TextKey::GAME_NEXT_PIECE] = {
        {Language::ENGLISH, "NEXT PIECE"},
        {Language::CZECH, "DALŠÍ KOSTKA"}
    };
    texts[TextKey::GAME_DIFFICULTY] = {
        {Language::ENGLISH, "DIFFICULTY"},
        {Language::CZECH, "OBTÍŽNOST"}
    };
    texts[TextKey::GAME_OVER_TITLE] = {
        {Language::ENGLISH, "GAME OVER"},
        {Language::CZECH, "GAME OVER"}
    };
    texts[TextKey::GAME_OVER_SCORE] = {
        {Language::ENGLISH, "Score"},
        {Language::CZECH, "Skóre"}
    };
    texts[TextKey::GAME_OVER_ESC] = {
        {Language::ENGLISH, "ESC = Menu"},
        {Language::CZECH, "ESC = Menu"}
    };

    // Difficulty Levels
    texts[TextKey::DIFFICULTY_EASY] = {
        {Language::ENGLISH, "Easy"},
        {Language::CZECH, "Lehká"}
    };
    texts[TextKey::DIFFICULTY_NORMAL] = {
        {Language::ENGLISH, "Normal"},
        {Language::CZECH, "Normální"}
    };
    texts[TextKey::DIFFICULTY_HARD] = {
        {Language::ENGLISH, "Hard"},
        {Language::CZECH, "Těžká"}
    };

    // FPS Counter
    texts[TextKey::FPS_COUNTER] = {
        {Language::ENGLISH, "FPS"},
        {Language::CZECH, "FPS"}
    };
}

const char* Localization::GetText(TextKey key) const {
    auto it = texts.find(key);
    if (it != texts.end()) {
        auto lang_it = it->second.find(current_language);
        if (lang_it != it->second.end()) {
            return lang_it->second.c_str();
        }
    }
    return "[Missing Translation]";
}

void Localization::SetLanguage(Language lang) {
    current_language = lang;
}

const char* Localization::GetLanguageName(Language lang) const {
    switch (lang) {
        case Language::ENGLISH: return "English";
        case Language::CZECH: return "Čeština";
        default: return "Unknown";
    }
}

const char* Localization::GetCurrentLanguageName() const {
    return GetLanguageName(current_language);
}
