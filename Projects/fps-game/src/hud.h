#pragma once
#include "raylib.h"
#include <string>

struct HUD {
    void DrawCrosshair() const;
    void DrawScore(int score, int kills) const;
    void DrawHitMarker(const std::string& zone, float timer) const;
    void DrawTargetHP(int targetCount, int aliveCount) const;
};
