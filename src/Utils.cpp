#include "Utils.hpp"
#include <algorithm>

Color BlendColors(Color c1, Color c2, float blend) {
    return {
        static_cast<unsigned char>(c1.r * (1 - blend) + c2.r * blend),
        static_cast<unsigned char>(c1.g * (1 - blend) + c2.g * blend),
        static_cast<unsigned char>(c1.b * (1 - blend) + c2.b * blend),
        255
    };
}

Color BrightenColor(Color c, float factor) {
    return {
        static_cast<unsigned char>(std::min(255.0f, c.r * factor)),
        static_cast<unsigned char>(std::min(255.0f, c.g * factor)),
        static_cast<unsigned char>(std::min(255.0f, c.b * factor)),
        c.a
    };
}

Color DarkenColor(Color c, float factor) {
    return {
        static_cast<unsigned char>(std::max(0.0f, c.r * factor)),
        static_cast<unsigned char>(std::max(0.0f, c.g * factor)),
        static_cast<unsigned char>(std::max(0.0f, c.b * factor)),
        c.a
    };
}

Color ColorWithAlpha(Color c, unsigned char alpha) {
    return {c.r, c.g, c.b, alpha};
}
