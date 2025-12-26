#pragma once

#include "raylib.h"

// Helper funkce
Color BlendColors(Color c1, Color c2, float blend);
Color BrightenColor(Color c, float factor);
Color DarkenColor(Color c, float factor);
Color ColorWithAlpha(Color c, unsigned char alpha);
