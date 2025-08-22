#include "ui.h"
#include "assets.h"
#include <stdio.h>

static void DrawCoralPiece(int x, int y, int size, CoralColor color)
{
    if (color == CORAL_NONE) {
        DrawRectangle(x, y, size, size, (Color){ 200, 200, 200, 100 });
        DrawRectangleLines(x, y, size, size, BLACK);
        return;
    }
    if (gAssets.coralLoaded[color]) {
        // Draw coral texture with full transparency support
        Rectangle src = { 0, 0, (float)gAssets.coral[color].width, (float)gAssets.coral[color].height };
        Rectangle dst = { (float)x, (float)y, (float)size, (float)size };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(gAssets.coral[color], src, dst, origin, 0.0f, WHITE);
    } else {
        // Fallback with semi-transparency
        Color fallbackColor = CORAL_COLOR_MAP[color];
        fallbackColor.a = 200;  // Semi-transparent fallback
        DrawRectangle(x, y, size, size, fallbackColor);
        DrawRectangleLines(x, y, size, size, BLACK);
    }
}

static void DrawPointToken(int x, int y, int points)
{
    // Draw circle with number for point tokens (scaled)
    int radius = 10;
    DrawCircle(x + radius, y + radius, radius, GOLD);
    DrawCircleLines(x + radius, y + radius, radius, BLACK);
    
    // Draw the number centered in the circle (scaled)
    const char* text = TextFormat("%d", points);
    int textWidth = MeasureText(text, 10);
    DrawText(text, x + radius - textWidth/2, y + radius - 5, 10, BLACK);
}

void UI_DrawBackground(void)
{
    ClearBackground(LIGHTGRAY);
}

void UI_DrawPlayerBoard(const Player* p, int ox, int oy, bool highlightValid, CoralColor placeColor)
{
    (void)highlightValid; // reserved for later phases
    (void)placeColor;

    // Draw board background/grid with player-specific border color
    Color boardBorderColor = (p->id == 0) ? BLUE : RED;
    DrawRectangleLines(ox, oy, UI_BOARD_SIZE, UI_BOARD_SIZE, boardBorderColor);
    
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            int x = ox + c * UI_CELL_SIZE;
            int y = oy + r * UI_CELL_SIZE;

            // Draw cell border
            DrawRectangleLines(x, y, UI_CELL_SIZE, UI_CELL_SIZE, GRAY);

            const CoralStack* s = &p->board[r][c];
            
            // Draw all coral pieces in the stack with transparency
            for (int stackLevel = 0; stackLevel < s->height; ++stackLevel) {
                CoralColor color = s->pieces[stackLevel];
                // Offset each stacked piece slightly to show layering (scaled down)
                int offsetX = stackLevel * 4;
                int offsetY = stackLevel * 4;
                DrawCoralPiece(x + offsetX, y + offsetY, UI_CELL_SIZE - offsetX, color);
            }
            
            // Draw stack height indicator if more than 1 (scaled text)
            if (s->height > 1) {
                DrawText(TextFormat("%d", s->height), x + UI_CELL_SIZE - 20, y + 5, 12, BLACK);
            }
        }
    }

    // Draw player board title with background to make ownership clear (scaled)
    Color playerColor = (p->id == 0) ? BLUE : RED;
    DrawRectangle(ox - 5, oy - 30, 200, 20, (Color){playerColor.r, playerColor.g, playerColor.b, 50});
    DrawText(TextFormat("Player %d Board", p->id + 1), ox, oy - 25, 16, playerColor);
    DrawText(TextFormat("Points: %d", p->points), ox, oy - 10, 14, BLACK);
}

void UI_DrawCard(const Card* c, int x, int y)
{
    if (gAssets.cardBgLoaded) {
        Rectangle src = { 0, 0, (float)gAssets.cardBg.width, (float)gAssets.cardBg.height };
        Rectangle dst = { (float)x, (float)y, (float)UI_CARD_W, (float)UI_CARD_H };
        DrawTexturePro(gAssets.cardBg, src, dst, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleLines(x, y, UI_CARD_W, UI_CARD_H, BLACK);
    } else {
        DrawRectangle(x, y, UI_CARD_W, UI_CARD_H, WHITE);
        DrawRectangleLines(x, y, UI_CARD_W, UI_CARD_H, BLACK);
    }

    int px = x + 8;
    int py = y + 8;
    DrawCoralPiece(px, py, 16, c->piece1);
    DrawCoralPiece(px + 20, py, 16, c->piece2);

    DrawText(TextFormat("%d", c->points), x + UI_CARD_W - 16, y + UI_CARD_H - 16, 12, BLACK);
}

void UI_DrawMarket(const GameState* g)
{
    DrawText("Market", UI_MARKET_X, UI_MARKET_Y - 18, 16, BLACK);

    for (int i = 0; i < CARD_DISPLAY_SIZE; ++i) {
        int x = UI_MARKET_X + i * (UI_CARD_W + UI_CARD_GAP);
        int y = UI_MARKET_Y;
        UI_DrawCard(&g->display[i], x, y);

        // Point tokens indicator - always draw as circles with numbers (scaled)
        int t = g->displayTokens[i];
        if (t > 0) {
            int tx = x + UI_CARD_W - 24;
            int ty = y + 4;
            DrawPointToken(tx, ty, t);
        }

        // Hotkey label (scaled)
        DrawText(TextFormat("%d", i + 1), x + 4, y + 4, 12, RED);
    }
}

void UI_DrawDeck(const GameState* g)
{
    (void)g;
    int x = UI_DECK_X;
    int y = UI_DECK_Y;

    if (gAssets.deckBackLoaded) {
        Rectangle src = { 0, 0, (float)gAssets.deckBack.width, (float)gAssets.deckBack.height };
        Rectangle dst = { (float)x, (float)y, (float)UI_CARD_W, (float)UI_CARD_H };
        DrawTexturePro(gAssets.deckBack, src, dst, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleLines(x, y, UI_CARD_W, UI_CARD_H, BLACK);
    } else {
        DrawRectangle(x, y, UI_CARD_W, UI_CARD_H, (Color){ 80, 80, 80, 255 });
        DrawRectangleLines(x, y, UI_CARD_W, UI_CARD_H, BLACK);
        DrawText("DECK", x + 22, y + 44, 14, RAYWHITE);
    }
}

void UI_DrawHand(const Player* p, int x, int y, int selectedIndex)
{
    // Highlight current player's hand title (scaled)
    Color titleColor = (selectedIndex == -1) ? RED : BLACK;
    const char* turnIndicator = (selectedIndex == -1) ? " (TURN)" : "";
    DrawText(TextFormat("P%d Hand%s", p->id + 1, turnIndicator), x, y - 18, 14, titleColor);
    
    for (int i = 0; i < p->handSize; ++i) {
        int cx = x + i * (UI_CARD_W + UI_CARD_GAP);
        UI_DrawCard(&p->hand[i], cx, y);
        if (i == selectedIndex) {
            DrawRectangleLines(cx - 2, y - 2, UI_CARD_W + 4, UI_CARD_H + 4, RED);
        }
        
        // Only show hotkeys for current player (scaled)
        if (selectedIndex == -1) {
            const char* key = (i == 0) ? "Q" : (i == 1) ? "W" : (i == 2) ? "E" : (i == 3) ? "R" : "";
            DrawText(key, cx + 4, y + 4, 12, RED);
        }
    }
}

void UI_DrawSupplies(const GameState* g)
{
    DrawText("Supplies", UI_SUPPLY_X, UI_SUPPLY_Y - 18, 16, BLACK);

    for (int i = 1; i <= 4; ++i) {
        int x = UI_SUPPLY_X + (i - 1) * 28;
        int y = UI_SUPPLY_Y;
        DrawCoralPiece(x, y, 20, (CoralColor)i);
        DrawText(TextFormat("%d", g->supplies[i]), x + 4, y + 22, 12, BLACK);
    }
}

void UI_DrawTopBar(const GameState* g)
{
    DrawText(TextFormat("Current Player: %d", g->currentPlayer + 1), 20, 20, 18, BLACK);
    DrawText("Actions: [1-3] Take Market | [D] Draw Deck (-1pt) | Play: [Q,W,E,R]", 20, 40, 12, BLACK);
}
