#ifndef UI_H
#define UI_H

#include "constants.h"

void UI_DrawBackground(void);
void UI_DrawPlayerBoard(const Player* p, int ox, int oy, bool highlightValid, CoralColor placeColor);
void UI_DrawCard(const Card* c, int x, int y);
void UI_DrawMarket(const GameState* g);
void UI_DrawDeck(const GameState* g);
void UI_DrawHand(const Player* p, int x, int y, int selectedIndex);
void UI_DrawSupplies(const GameState* g);
void UI_DrawTopBar(const GameState* g);

#endif
