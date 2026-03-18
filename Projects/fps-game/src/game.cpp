#include "game.h"

void Game::Init() {
    player.Init();
    map.Init();
    SpawnTargets();
}

void Game::SpawnTargets() {
    targets.clear();

    // Each target gets a start position and a list of patrol waypoints
    struct Spawn { Vector3 pos; std::vector<Vector3> wps; };

    std::vector<Spawn> spawns = {
        // Target 1 — patrols around centre divider
        { {  0.0f, 0.0f,  4.0f }, {
            {  0.0f, 0.0f,  4.0f },
            {  2.5f, 0.0f,  6.0f },
            {  0.0f, 0.0f,  8.5f },
            { -2.5f, 0.0f,  6.0f }
        }},
        // Target 2 — patrols left side
        { { -5.0f, 0.0f,  5.0f }, {
            { -5.0f, 0.0f,  5.0f },
            { -7.0f, 0.0f,  7.5f },
            { -8.0f, 0.0f,  3.0f },
            { -6.0f, 0.0f,  1.0f }
        }},
        // Target 3 — patrols right side
        { {  5.0f, 0.0f,  3.5f }, {
            {  5.0f, 0.0f,  3.5f },
            {  7.5f, 0.0f,  6.5f },
            {  8.5f, 0.0f,  2.0f },
            {  6.5f, 0.0f, -0.5f }
        }},
        // Target 4 — patrols far back left
        { { -3.5f, 0.0f, -1.5f }, {
            { -3.5f, 0.0f, -1.5f },
            { -6.0f, 0.0f, -4.0f },
            { -2.0f, 0.0f, -7.0f },
            { -0.5f, 0.0f, -4.0f }
        }},
        // Target 5 — patrols far back right
        { {  4.5f, 0.0f,  0.0f }, {
            {  4.5f, 0.0f,  0.0f },
            {  7.0f, 0.0f, -2.0f },
            {  5.0f, 0.0f, -6.0f },
            {  2.5f, 0.0f, -3.5f }
        }},
    };

    for (auto& s : spawns) {
        Target t;
        t.Init(s.pos, s.wps);
        targets.push_back(t);
    }
}

void Game::Update(float dt) {
    player.Update(dt, map);
    weapon.Update(dt);

    // Fire on left mouse button
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        HitResult res = weapon.TryFire(player, targets, map);
        if (res.hit) {
            score += res.score;
            if (res.killed) kills++;
        }
    }

    for (auto& t : targets)
        t.Update(dt, player, map);
}

void Game::Draw() {
    Camera3D cam = player.GetCamera();

    // ── 3D world ────────────────────────────────────────────────────────────
    BeginMode3D(cam);

    map.Draw();

    for (auto& t : targets)
        t.Draw();

    EndMode3D();

    // ── 2D HUD (drawn on top) ────────────────────────────────────────────────
    weapon.DrawGun();
    weapon.DrawMuzzleFlash();
    hud.DrawCrosshair();
    hud.DrawScore(score, kills);
    hud.DrawHitMarker(weapon.lastHit.zone, weapon.hitMarkerTimer);
    hud.DrawTargetHP((int)targets.size(), AliveCount());
}

int Game::AliveCount() const {
    int n = 0;
    for (const auto& t : targets) if (t.alive) n++;
    return n;
}
