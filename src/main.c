#include "game.h"
#include "assets.h"

int main(void)
{
    GameState g = {0};
    GameInit(&g);

    while (!WindowShouldClose() && !g.gameEnded) {
        GameUpdate(&g);
        BeginDrawing();
        GameDraw(&g);
        EndDrawing();
    }

    AssetsUnloadAll();
    CloseWindow();
    return 0;
}
