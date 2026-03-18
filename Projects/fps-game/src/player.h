#pragma once
#include "raylib.h"
#include "raymath.h"
#include "map.h"

struct Player {
    Vector3 position = { 0.0f, 0.0f, -9.0f };
    float   yaw      = 0.0f;   // radians; 0 = looking toward +Z
    float   pitch    = 0.0f;   // radians; clamped to +-89 deg

    static constexpr float EYE_HEIGHT  = 1.65f;
    static constexpr float MOVE_SPEED  = 6.0f;
    static constexpr float SENSITIVITY = 0.002f;
    static constexpr float RADIUS      = 0.35f;

    void Init();
    void Update(float dt, const Map& map);

    Camera3D GetCamera()      const;
    BoundingBox GetBounds()   const;
    Vector3 GetForward()      const; // pitch-aware (for shooting ray)
    Vector3 GetFlatForward()  const; // XZ only (for movement)
    Vector3 GetRight()        const; // XZ only (for movement)

private:
    BoundingBox BoundsAt(Vector3 pos) const;
    void HandleLook();
    void HandleMove(float dt, const Map& map);
};
