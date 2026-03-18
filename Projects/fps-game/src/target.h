#pragma once
#include "raylib.h"
#include "raymath.h"
#include "map.h"
#include "player.h"
#include <vector>
#include <string>

enum class AIState { Patrol, Alert, SeekCover, InCover };

struct Hitbox {
    BoundingBox box;
    int  damage;
    bool instantKill;
    std::string zone; // "HEAD", "TORSO", "LEGS"
};

struct Target {
    Vector3 position;   // root = feet centre
    int     hp;
    bool    alive;
    float   respawnTimer;

    AIState state;
    float   stateTimer;

    std::vector<Vector3> waypoints;
    int     waypointIdx;
    Vector3 coverPos;

    Hitbox head, torso, legs;

    static constexpr float RADIUS        = 0.28f;
    static constexpr float HEIGHT        = 1.8f;
    static constexpr float WALK_SPEED    = 2.5f;
    static constexpr float RESPAWN_TIME  = 5.0f;
    static constexpr float COVER_STAY    = 4.0f;

    void Init(Vector3 startPos, std::vector<Vector3> pts);
    void Update(float dt, const Player& player, const Map& map);
    void Draw() const;
    void TakeDamage(int dmg, bool kill);

    BoundingBox GetBounds() const { return BoundsAt(position); }

private:
    void UpdateHitboxes();
    BoundingBox BoundsAt(Vector3 pos) const;

    void MoveToward(Vector3 dest, float speed, float dt, const Map& map);
    Vector3 PickCoverPos(const Player& player, const Map& map) const;

    void StatePatrol(float dt, const Player& player, const Map& map);
    void StateAlert (float dt, const Player& player, const Map& map);
    void StateSeekCover(float dt, const Player& player, const Map& map);
    void StateInCover(float dt, const Player& player, const Map& map);
};
