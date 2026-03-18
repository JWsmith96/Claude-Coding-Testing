#include "hud.h"
#include <string>

void HUD::DrawCrosshair() const {
    int cx = GetScreenWidth()  / 2;
    int cy = GetScreenHeight() / 2;
    int gap = 5, len = 10, thick = 2;
    Color col = { 200, 200, 200, 200 };

    DrawRectangle(cx - gap - len, cy - thick/2, len,  thick, col); // left
    DrawRectangle(cx + gap,       cy - thick/2, len,  thick, col); // right
    DrawRectangle(cx - thick/2,   cy - gap - len, thick, len, col); // top
    DrawRectangle(cx - thick/2,   cy + gap,       thick, len, col); // bottom
    DrawRectangle(cx - 1,         cy - 1,          3,    3,   col); // centre dot
}

void HUD::DrawScore(int score, int kills) const {
    DrawText(TextFormat("SCORE: %d", score),  12, 12, 22, { 240, 200, 60, 255 });
    DrawText(TextFormat("KILLS: %d", kills),  12, 38, 18, { 180, 180, 180, 200 });

    // Controls reminder (faded, small)
    int sh = GetScreenHeight();
    DrawText("WASD Move  |  Mouse Look  |  LMB Shoot  |  ESC Quit",
             12, sh - 24, 14, { 120, 120, 120, 160 });
}

void HUD::DrawHitMarker(const std::string& zone, float timer) const {
    if (timer <= 0.0f) return;

    int   cx   = GetScreenWidth()  / 2;
    int   cy   = GetScreenHeight() / 2;
    float fade = timer / 0.4f;  // 0→1
    unsigned char alpha = (unsigned char)(255 * fade);

    Color col;
    if      (zone == "HEAD")  col = { 255, 60,  60,  alpha };
    else if (zone == "TORSO") col = { 255, 160, 60,  alpha };
    else                      col = { 200, 200, 200, alpha };

    // X-shaped hit marker
    int s = 8;
    DrawLineEx({ (float)(cx-s), (float)(cy-s) }, { (float)(cx+s), (float)(cy+s) }, 2.5f, col);
    DrawLineEx({ (float)(cx+s), (float)(cy-s) }, { (float)(cx-s), (float)(cy+s) }, 2.5f, col);

    // Zone label
    const char* label = zone.c_str();
    int tw = MeasureText(label, 16);
    DrawText(label, cx - tw/2, cy - 36, 16, col);
}

void HUD::DrawTargetHP(int targetCount, int aliveCount) const {
    int sw = GetScreenWidth();
    DrawText(TextFormat("Targets: %d / %d", aliveCount, targetCount),
             sw - 180, 12, 16, { 160, 200, 160, 200 });
}
