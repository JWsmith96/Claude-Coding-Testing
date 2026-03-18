#pragma once
#include "raylib.h"
#include <vector>

struct Obstacle {
    Vector3 position;   // XZ center, Y = bottom of obstacle
    Vector3 size;
    Color   color;
    bool    isCoverPoint; // AI can hide behind this

    BoundingBox GetBounds() const {
        return {
            { position.x - size.x * 0.5f,  position.y,            position.z - size.z * 0.5f },
            { position.x + size.x * 0.5f,  position.y + size.y,   position.z + size.z * 0.5f }
        };
    }
};

struct Map {
    std::vector<Obstacle> obstacles;

    void Init();
    void Draw() const;

    // Returns true if 'box' overlaps any obstacle
    bool CollidesWithAny(const BoundingBox& box) const;

    // Returns true if ray hits any obstacle; sets outDist to nearest hit
    bool RayHitsAny(const Ray& ray, float& outDist) const;
};
