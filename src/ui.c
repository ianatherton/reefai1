#include "ui.h"
#include "assets.h"
#include "constants.h"
#include "patterns.h"
#include <stdio.h>

// Helper function to draw text with custom font and 10% larger size
static void DrawTextCustom(const char* text, int posX, int posY, int baseFontSize, Color color)
{
    int scaledSize = (int)(baseFontSize * 1.3f); // 10% larger
    if (gAssets.fontLoaded) {
        DrawTextEx(gAssets.customFont, text, (Vector2){posX, posY}, scaledSize, 1.0f, color);
    } else {
        // Fallback to default font with scaled size
        DrawText(text, posX, posY, scaledSize, color);
    }
}

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
    DrawTextCustom(text, x + radius - textWidth/2, y + radius - 5, 10, BLACK);
}

void UI_DrawBackground(void)
{
    ClearBackground(LIGHTGRAY);
}

void UI_DrawPlayerBoard(const Player* p, int ox, int oy, bool highlightValid, CoralColor placeColor)
{
    // Draw gameboard background texture if available (scaled from 1024x1024 to 512x512)
    if (gAssets.gameboardLoaded) {
        Rectangle src = { 0, 0, (float)gAssets.gameboard.width, (float)gAssets.gameboard.height };
        Rectangle dst = { (float)ox, (float)oy, (float)UI_BOARD_SIZE, (float)UI_BOARD_SIZE };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(gAssets.gameboard, src, dst, origin, 0.0f, WHITE);
    }
    
    // Draw board background/grid with player-specific border color
    Color boardBorderColor = (p->id == 0) ? BLUE : RED;
    DrawRectangleLines(ox, oy, UI_BOARD_SIZE, UI_BOARD_SIZE, boardBorderColor);
    
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            int x = ox + c * UI_CELL_SIZE;
            int y = oy + r * UI_CELL_SIZE;

            const CoralStack* s = &p->board[r][c];
            
            // Highlight valid placement positions
            if (highlightValid && s->height < MAX_STACK_HEIGHT) {
                DrawRectangle(x, y, UI_CELL_SIZE, UI_CELL_SIZE, (Color){0, 255, 0, 50});
            }
            
            // Draw cell border
            Color cellBorderColor = GRAY;
            if (highlightValid && s->height < MAX_STACK_HEIGHT) {
                cellBorderColor = GREEN;
            }
            DrawRectangleLines(x, y, UI_CELL_SIZE, UI_CELL_SIZE, cellBorderColor);

            // Draw all coral pieces in the stack with transparency
            for (int stackLevel = 0; stackLevel < s->height; ++stackLevel) {
                CoralColor color = s->pieces[stackLevel];
                // Offset each stacked piece by 15 pixels to show layering clearly (scaled for smaller cells)
                int offsetY = stackLevel * 15;
                DrawCoralPiece(x, y - offsetY, UI_CELL_SIZE, color);
            }
            
            // Preview the coral piece being placed
            if (highlightValid && placeColor != CORAL_NONE && s->height < MAX_STACK_HEIGHT) {
                int previewOffsetY = s->height * 15;
                Color previewTint = CORAL_COLOR_MAP[placeColor];
                previewTint.a = 100; // Semi-transparent preview
                DrawRectangle(x + 8, y - previewOffsetY + 8, 
                             UI_CELL_SIZE - 16, UI_CELL_SIZE - 16, previewTint);
            }
            
            // Draw stack height indicator if more than 1 (bigger font for visibility)
            if (s->height > 1) {
                DrawTextCustom(TextFormat("%d", s->height), x + UI_CELL_SIZE - 18, y + 5, 16, BLACK);
            }
        }
    }

    // Draw player board title with background to make ownership clear (scaled)
    Color playerColor = (p->id == 0) ? BLUE : RED;
    DrawRectangle(ox - 5, oy - 30, 200, 20, (Color){playerColor.r, playerColor.g, playerColor.b, 50});
    DrawTextCustom(TextFormat("Player %d Board", p->id + 1), ox, oy - 25, 16, playerColor);
    DrawTextCustom(TextFormat("Points: %d", p->points), ox, oy - 10, 14, BLACK);
}

static void DrawScoringPattern(const ScoringPattern* pattern, int x, int y, int cellSize)
{
    // Draw the scoring pattern in the bottom half of the card
    for (int r = 0; r < pattern->height; r++) {
        for (int c = 0; c < pattern->width; c++) {
            const PatternCell* cell = &pattern->cells[r][c];
            
            // Skip empty pattern cells
            if (cell->color == CORAL_NONE && cell->exactHeight == 0 && cell->minHeight == 0 && !cell->isWild) {
                continue;
            }
            
            int cellX = x + c * cellSize;
            int cellY = y + r * cellSize;
            
            // Draw pattern cell background
            Color cellColor = LIGHTGRAY;
            if (cell->color != CORAL_NONE) {
                cellColor = CORAL_COLOR_MAP[cell->color];
                cellColor.a = 150; // Semi-transparent
            } else if (cell->isWild) {
                cellColor = (Color){128, 128, 128, 150}; // Gray for wild
            }
            
            DrawRectangle(cellX, cellY, cellSize, cellSize, cellColor);
            DrawRectangleLines(cellX, cellY, cellSize, cellSize, BLACK);
            
            // Draw height indicator if specified
            if (cell->exactHeight > 0) {
                DrawTextCustom(TextFormat("%d", cell->exactHeight), cellX + 2, cellY + 2, 8, BLACK);
            } else if (cell->minHeight > 0) {
                DrawTextCustom(TextFormat("%d+", cell->minHeight), cellX + 2, cellY + 2, 8, BLACK);
            }
        }
    }
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

    // Draw dividing line between top and bottom halves
    int midY = y + UI_CARD_H / 2;
    DrawLine(x + 4, midY, x + UI_CARD_W - 4, midY, BLACK);
    
    // Top half: Coral pieces to collect
    int px = x + 8;
    int py = y + 8;
    DrawCoralPiece(px, py, 16, c->piece1);
    DrawCoralPiece(px + 20, py, 16, c->piece2);

    // Bottom half: Scoring pattern
    int patternX = x + 8;
    int patternY = midY + 8;
    int patternCellSize = 12; // Small cells for pattern display
    DrawScoringPattern(&c->pattern, patternX, patternY, patternCellSize);
    
    // Points value in bottom right
    DrawTextCustom(TextFormat("%d", c->pattern.pointValue), x + UI_CARD_W - 16, y + UI_CARD_H - 16, 12, BLACK);
}

void UI_DrawMarket(const GameState* g)
{
    DrawTextCustom("Market", UI_MARKET_X, UI_MARKET_Y - 18, 16, BLACK);

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
        DrawTextCustom(TextFormat("%d", i + 1), x + 4, y + 4, 12, RED);
    }
}

void UI_DrawDeck(const GameState* g)
{
    int x = UI_DECK_X;
    int y = UI_DECK_Y;

    // Draw deck back pile
    if (gAssets.deckBackLoaded) {
        Rectangle src = { 0, 0, (float)gAssets.deckBack.width, (float)gAssets.deckBack.height };
        Rectangle dst = { (float)x, (float)y, (float)UI_CARD_W, (float)UI_CARD_H };
        DrawTexturePro(gAssets.deckBack, src, dst, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleLines(x, y, UI_CARD_W, UI_CARD_H, BLACK);
    } else {
        DrawRectangle(x, y, UI_CARD_W, UI_CARD_H, (Color){ 80, 80, 80, 255 });
        DrawRectangleLines(x, y, UI_CARD_W, UI_CARD_H, BLACK);
        DrawTextCustom("DECK", x + 22, y + 44, 14, RAYWHITE);
    }
    
    // Draw face-up card from top of deck 20px above the deck pile for stacking effect
    if (g->deckSize > 0) {
        int faceUpX = x;
        int faceUpY = y - 20;  // 20px above deck pile
        UI_DrawCard(&g->deck[g->deckSize - 1], faceUpX, faceUpY);
        
        // Draw hotkey label for deck card
        DrawTextCustom("D", faceUpX + 4, faceUpY + 4, 12, RED);
        
        // Show cost indicator
        DrawTextCustom("(-1pt)", faceUpX + 4, faceUpY + UI_CARD_H - 16, 10, RED);
    }
}

void UI_DrawHand(const Player* p, int x, int y, int selectedIndex)
{
    // Highlight current player's hand title (scaled)
    Color titleColor = (selectedIndex == -1) ? RED : BLACK;
    const char* turnIndicator = (selectedIndex == -1) ? " (TURN)" : "";
    DrawTextCustom(TextFormat("P%d Hand%s", p->id + 1, turnIndicator), x, y - 18, 14, titleColor);
    
    for (int i = 0; i < p->handSize; ++i) {
        int cx = x + i * (UI_CARD_W + UI_CARD_GAP);
        UI_DrawCard(&p->hand[i], cx, y);
        if (i == selectedIndex) {
            DrawRectangleLines(cx - 2, y - 2, UI_CARD_W + 4, UI_CARD_H + 4, RED);
        }
        
        // Only show hotkeys for current player (scaled)
        if (selectedIndex == -1) {
            const char* key = (i == 0) ? "Q" : (i == 1) ? "W" : (i == 2) ? "E" : (i == 3) ? "R" : "";
            DrawTextCustom(key, cx + 4, y + 4, 12, RED);
        }
    }
}

void UI_DrawSupplies(const GameState* g)
{
    DrawTextCustom("Supplies", UI_SUPPLY_X, UI_SUPPLY_Y - 18, 16, BLACK);

    for (int i = 1; i <= 4; ++i) {
        int x = UI_SUPPLY_X + (i - 1) * 28;
        int y = UI_SUPPLY_Y;
        DrawCoralPiece(x, y, 20, (CoralColor)i);
        DrawTextCustom(TextFormat("%d", g->supplies[i]), x + 4, y + 22, 12, BLACK);
    }
}

void UI_DrawTopBar(const GameState* g)
{
    DrawTextCustom(TextFormat("Current Player: %d", g->currentPlayer + 1), 20, 20, 18, BLACK);
    
    if (g->placement.active) {
        CoralColor currentColor = g->placement.piecesToPlace[g->placement.piecesPlaced];
        const char* colorName = CORAL_COLOR_NAME[currentColor];
        DrawTextCustom(TextFormat("PLACEMENT MODE: Click to place %s coral (%d/2)", 
                 colorName, g->placement.piecesPlaced + 1), 20, 40, 14, RED);
        
        // Draw a preview of the coral being placed
        DrawCoralPiece(400, 35, 20, currentColor);
    } else {
        DrawTextCustom("Actions: [1-3] Take Market | [D] Draw Deck (-1pt) | Play: [Q,W,E,R]", 20, 40, 12, BLACK);
    }
}
