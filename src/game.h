#ifndef GAME_H
#define GAME_H

#include "constants.h"

// Game initialization and lifecycle
void GameInit(GameState* g);
void GameUpdate(GameState* g);
void GameDraw(const GameState* g);

#endif
