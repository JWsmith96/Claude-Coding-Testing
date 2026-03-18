#include "target.h"
#include <cmath>
#include <cfloat>

// ── Init ─────────────────────────────────────────────────────────────────────

void Target::Init(Vector3 startPos, std::vector<Vector3> pts) {
    position     = startPos;
    hp           = 3;
    alive        = true;
    respawnTimer = 0.0f;
    state        = AIState::Patrol;
    stateTimer   = 0.0f;
    waypoints    = pts;
    waypointIdx  = 0;
    coverPos     = startPos;

    UpdateHitboxes();
}

// ── Update ────────────────────────────────────────────────────────────────────

void Target::Update(float dt, const Player& player, const Map& map) {
    if (!alive) {
        respawnTimer -= dt;
        if (respawnTimer <= 0.0f) {
            alive = true;
            hp    = 3;
            state = AIState::Patrol;
            stateTimer = 0.0f;
            position = waypoints.empty() ? position : waypoints[0];
        }
        return;
    }

    switch (state) {
        case AIState::Patrol:    StatePatrol   (dt, player, map); break;
        case AIState::Alert:     StateAlert    (dt, player, map); break;
        case AIState::SeekCover: StateSeekCover(dt, player, map); break;
        case AIState::InCover:   StateInCover  (dt, player, map); break;
    }

    UpdateHitboxes();
}

// ── AI States ─────────────────────────────────────────────────────────────────

void Target::StatePatrol(float dt, const Player& player, const Map& map) {
    if (waypoints.empty()) return;

    Vector3 dest = waypoints[waypointIdx];
    MoveToward(dest, WALK_SPEED, dt, map);

    if (Vector3Distance(position, dest) < 0.4f) {
        waypointIdx = (waypointIdx + 1) % (int)waypoints.size();
    }
    (void)player;
}

void Target::StateAlert(float dt, const Player& player, const Map& map) {
    stateTimer -= dt;
    if (stateTimer <= 0.0f) {
        coverPos   = PickCoverPos(player, map);
        state      = AIState::SeekCover;
        stateTimer = 0.0f;
    }
    (void)map;
}

void Target::StateSeekCover(float dt, const Player& player, const Map& map) {
    MoveToward(coverPos, WALK_SPEED * 1.4f, dt, map);

    if (Vector3Distance(position, coverPos) < 0.5f) {
        state      = AIState::InCover;
        stateTimer = COVER_STAY;
    }
    (void)player;
}

void Target::StateInCover(float dt, const Player& player, const Map& map) {
    stateTimer -= dt;
    if (stateTimer <= 0.0f) {
        state = AIState::Patrol;
    }
    (void)player; (void)map;
}

// ── Movement ──────────────────────────────────────────────────────────────────

void Target::MoveToward(Vector3 dest, float speed, float dt, const Map& map) {
    Vector3 dir = Vector3Subtract(dest, position);
    float dist  = Vector3Length(dir);
    if (dist < 0.01f) return;

    dir = Vector3Scale(dir, 1.0f / dist);
    float step = fminf(speed * dt, dist);
    Vector3 move = Vector3Scale(dir, step);

    // Direct move
    Vector3 np = Vector3Add(position, move);
    if (!map.CollidesWithAny(BoundsAt(np))) {
        position = np;
        return;
    }
    // Wall-slide X
    Vector3 nx = { position.x + move.x, position.y, position.z };
    if (!map.CollidesWithAny(BoundsAt(nx))) { position.x = nx.x; return; }
    // Wall-slide Z
    Vector3 nz = { position.x, position.y, position.z + move.z };
    if (!map.CollidesWithAny(BoundsAt(nz))) { position.z = nz.z; return; }
    // Deflect 45 deg
    float a = 45.0f * DEG2RAD;
    Vector3 def = {
        dir.x * cosf(a) - dir.z * sinf(a), 0,
        dir.x * sinf(a) + dir.z * cosf(a)
    };
    Vector3 nd = Vector3Add(position, Vector3Scale(def, step));
    if (!map.CollidesWithAny(BoundsAt(nd))) position = nd;
}

Vector3 Target::PickCoverPos(const Player& player, const Map& map) const {
    Vector3 best  = position;
    float bestD   = FLT_MAX;

    for (const auto& obs : map.obstacles) {
        if (!obs.isCoverPoint) continue;

        Vector3 toObs = Vector3Subtract(obs.position, player.position);
        float   len   = Vector3Length(toObs);
        if (len < 0.01f) continue;

        // Point on the far side of this obstacle from the player
        Vector3 dir   = Vector3Scale(toObs, 1.0f / len);
        float   depth = fmaxf(obs.size.x, obs.size.z) * 0.5f + 0.9f;
        Vector3 cand  = Vector3Add(obs.position, Vector3Scale(dir, depth));
        cand.y        = position.y;

        // Prefer the closest candidate to us
        float d = Vector3Distance(cand, position);
        if (d < bestD) { bestD = d; best = cand; }
    }
    return best;
}

// ── Damage ────────────────────────────────────────────────────────────────────

void Target::TakeDamage(int dmg, bool kill) {
    if (!alive) return;
    if (kill) hp = 0;
    else      hp -= dmg;

    if (hp <= 0) {
        alive        = false;
        hp           = 0;
        respawnTimer = RESPAWN_TIME;
    } else {
        // React: go to alert if not already seeking cover
        if (state == AIState::Patrol) {
            state      = AIState::Alert;
            stateTimer = 0.25f; // 250ms reaction
        }
    }
}

// ── Rendering ─────────────────────────────────────────────────────────────────

void Target::Draw() const {
    if (!alive) return;

    // Legs (cylinder): centre of base at feet
    DrawCylinder(position, RADIUS, RADIUS, 0.9f, 8,
                 { 80, 80, 200, 255 });
    DrawCylinderWires(position, RADIUS, RADIUS, 0.9f, 8, BLACK);

    // Torso (cube): sits on top of legs
    Vector3 torsoBase   = { position.x, position.y + 0.9f, position.z };
    Vector3 torsoCentre = { torsoBase.x, torsoBase.y + 0.3f, torsoBase.z };
    DrawCube(torsoCentre, 0.5f, 0.6f, 0.3f, { 60, 160, 60, 255 });
    DrawCubeWires(torsoCentre, 0.5f, 0.6f, 0.3f, BLACK);

    // Head (sphere)
    Vector3 headCentre = { position.x, position.y + 1.65f, position.z };
    DrawSphere(headCentre, 0.2f, { 220, 180, 140, 255 });

    // HP bar above head (3 segments)
    // Drawn as 3D billboard-ish using DrawLine via screen coords — done in HUD instead
}

// ── Hitboxes ──────────────────────────────────────────────────────────────────

void Target::UpdateHitboxes() {
    // Legs: y 0 – 0.9
    legs = {
        {
            { position.x - RADIUS,   position.y,        position.z - RADIUS },
            { position.x + RADIUS,   position.y + 0.9f, position.z + RADIUS }
        },
        1, false, "LEGS"
    };
    // Torso: y 0.9 – 1.5
    torso = {
        {
            { position.x - 0.26f,    position.y + 0.9f, position.z - 0.16f },
            { position.x + 0.26f,    position.y + 1.5f, position.z + 0.16f }
        },
        2, false, "TORSO"
    };
    // Head: y 1.45 – 1.85 (sphere radius 0.2 centred at 1.65)
    head = {
        {
            { position.x - 0.22f,    position.y + 1.45f, position.z - 0.22f },
            { position.x + 0.22f,    position.y + 1.85f, position.z + 0.22f }
        },
        3, true, "HEAD"
    };
}

BoundingBox Target::BoundsAt(Vector3 pos) const {
    return {
        { pos.x - RADIUS, pos.y,        pos.z - RADIUS },
        { pos.x + RADIUS, pos.y + HEIGHT, pos.z + RADIUS }
    };
}
