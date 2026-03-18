#include "map.h"

void Map::Init() {
    obstacles.clear();

    // ── Outer boundary walls (not usable as cover) ──────────────────────────
    // North wall
    obstacles.push_back({ {  0.0f, 0.0f,  12.5f }, { 26.0f, 3.5f, 1.0f }, DARKGRAY, false });
    // South wall
    obstacles.push_back({ {  0.0f, 0.0f, -12.5f }, { 26.0f, 3.5f, 1.0f }, DARKGRAY, false });
    // East wall
    obstacles.push_back({ {  12.5f, 0.0f, 0.0f  }, { 1.0f, 3.5f, 26.0f }, DARKGRAY, false });
    // West wall
    obstacles.push_back({ { -12.5f, 0.0f, 0.0f  }, { 1.0f, 3.5f, 26.0f }, DARKGRAY, false });

    // ── Interior cover obstacles ─────────────────────────────────────────────
    // Centre divider
    obstacles.push_back({ {  0.0f, 0.0f,  1.0f  }, { 6.0f, 2.0f, 0.8f }, { 120,100,80,255 }, true });
    // Left-side box
    obstacles.push_back({ { -4.0f, 0.0f,  4.0f  }, { 2.0f, 2.0f, 2.0f }, { 100,120,90,255 }, true });
    // Right-side box
    obstacles.push_back({ {  4.0f, 0.0f,  4.5f  }, { 2.0f, 2.0f, 2.0f }, { 90,110,130,255 }, true });
    // Far-left cover
    obstacles.push_back({ { -6.0f, 0.0f,  7.0f  }, { 2.0f, 2.0f, 2.0f }, { 110,90,100,255 }, true });
    // Far-right cover
    obstacles.push_back({ {  6.0f, 0.0f,  7.5f  }, { 2.0f, 2.0f, 2.0f }, { 100,100,120,255 }, true });
    // Long wall fragment left
    obstacles.push_back({ { -5.5f, 0.0f, -2.0f  }, { 1.0f, 2.0f, 4.0f }, { 90,100,110,255 }, true });
    // Long wall fragment right
    obstacles.push_back({ {  5.5f, 0.0f, -3.0f  }, { 3.5f, 2.0f, 0.8f }, { 110,100,90,255 }, true });
}

void Map::Draw() const {
    // Floor
    DrawPlane({ 0.0f, 0.0f, 0.0f }, { 25.0f, 25.0f }, { 55, 55, 55, 255 });

    for (const auto& obs : obstacles) {
        // DrawCube expects the CENTER of the cube
        Vector3 centre = { obs.position.x, obs.position.y + obs.size.y * 0.5f, obs.position.z };
        DrawCube(centre, obs.size.x, obs.size.y, obs.size.z, obs.color);
        DrawCubeWires(centre, obs.size.x, obs.size.y, obs.size.z, { 0,0,0,60 });
    }
}

bool Map::CollidesWithAny(const BoundingBox& box) const {
    for (const auto& obs : obstacles)
        if (CheckCollisionBoxes(box, obs.GetBounds())) return true;
    return false;
}

bool Map::RayHitsAny(const Ray& ray, float& outDist) const {
    bool   hit  = false;
    float  best = 1e30f;
    for (const auto& obs : obstacles) {
        RayCollision rc = GetRayCollisionBox(ray, obs.GetBounds());
        if (rc.hit && rc.distance > 0.01f && rc.distance < best) {
            best = rc.distance;
            hit  = true;
        }
    }
    outDist = best;
    return hit;
}
