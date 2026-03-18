#pragma once
#include "player.h"
#include "map.h"
#include "target.h"
#include "weapon.h"
#include "hud.h"
#include <vector>

struct Game {
    Player              player;
    Map                 map;
    std::vector<Target> targets;
    Weapon              weapon;
    HUD                 hud;

    int score = 0;
    int kills = 0;

    void Init();
    void Update(float dt);
    void Draw();

private:
    void SpawnTargets();
    int  AliveCount() const;
};
