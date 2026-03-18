#include "raylib.h"
#include "game.h"

int main() {
    InitWindow(1280, 720, "FPS Game");
    SetTargetFPS(60);
    DisableCursor();

    Game game;
    game.Init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        game.Update(dt);

        BeginDrawing();
        ClearBackground({ 20, 20, 35, 255 }); // dark sky
        game.Draw();
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
