#ifndef CARDS_H
#define CARDS_H

#include "constants.h"

void CardsInitAndShuffle(GameState* g);
void DisplayInit(GameState* g);
void DisplayRefillSlot(GameState* g, int index);
void DealInitialHands(GameState* g);
int  FindDisplayLowestPointsIndex(const GameState* g);

#endif
