#include <stdlib.h>
#include <time.h>
#include "cards.h"
#include "patterns.h"

static void ShuffleDeckInternal(Card* deck, int n)
{
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Card tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

// Create a variety of realistic Reef cards based on the image
static Card CreateReefCard(int cardIndex) {
    Card card = {0};
    
    // Define card templates based on the real Reef cards shown in the image
    CoralColor colors[] = {CORAL_YELLOW, CORAL_ORANGE, CORAL_PURPLE, CORAL_GREEN};
    
    switch (cardIndex % 15) {
        case 0: // Green pieces -> 2x2 yellow square (4 points)
            card.piece1 = CORAL_GREEN;
            card.piece2 = CORAL_GREEN;
            card.pattern = CreateSquare2x2Pattern(CORAL_YELLOW, 4);
            break;
            
        case 1: // Purple pieces -> 3 purple line (2 points)
            card.piece1 = CORAL_PURPLE;
            card.piece2 = CORAL_PURPLE;
            card.pattern = CreateLine3Pattern(CORAL_PURPLE, 2);
            break;
            
        case 2: // Orange pieces -> height-2 purple (1 point)
            card.piece1 = CORAL_ORANGE;
            card.piece2 = CORAL_ORANGE;
            card.pattern = CreateHeight2Pattern(CORAL_PURPLE, 1);
            break;
            
        case 3: // Yellow pieces -> height-2 any color (1 point)
            card.piece1 = CORAL_YELLOW;
            card.piece2 = CORAL_YELLOW;
            card.pattern = CreateHeight2Pattern(CORAL_NONE, 1);
            card.pattern.cells[0][0].isWild = true;
            break;
            
        case 4: // Green pieces -> 2 green line (2 points)
            card.piece1 = CORAL_GREEN;
            card.piece2 = CORAL_GREEN;
            card.pattern = CreateLine2Pattern(CORAL_GREEN, 2);
            break;
            
        case 5: // Yellow + Green -> L-shape yellow (5 points)
            card.piece1 = CORAL_YELLOW;
            card.piece2 = CORAL_GREEN;
            card.pattern = CreateLShapePattern(CORAL_YELLOW, 5);
            break;
            
        case 6: // Green pieces -> height-3 green (5 points)
            card.piece1 = CORAL_GREEN;
            card.piece2 = CORAL_GREEN;
            card.pattern = CreateHeight3Pattern(CORAL_GREEN, 5);
            break;
            
        case 7: // Yellow + Orange -> 2x2 orange (2 points)
            card.piece1 = CORAL_YELLOW;
            card.piece2 = CORAL_ORANGE;
            card.pattern = CreateSquare2x2Pattern(CORAL_ORANGE, 2);
            break;
            
        case 8: // Purple pieces -> height-2+ purple (8 points)
            card.piece1 = CORAL_PURPLE;
            card.piece2 = CORAL_PURPLE;
            card.pattern = CreateHeight2Pattern(CORAL_PURPLE, 8);
            card.pattern.cells[0][0].minHeight = 2;
            card.pattern.cells[0][0].exactHeight = 0;
            break;
            
        case 9: // Orange + Red -> 3 orange line (4 points)
            card.piece1 = CORAL_ORANGE;
            card.piece2 = CORAL_ORANGE;
            card.pattern = CreateLine3Pattern(CORAL_ORANGE, 4);
            break;
            
        case 10: // Green pieces -> 2 green line (1 point)
            card.piece1 = CORAL_GREEN;
            card.piece2 = CORAL_GREEN;
            card.pattern = CreateLine2Pattern(CORAL_GREEN, 1);
            break;
            
        case 11: // Purple pieces -> 2x2 orange (8 points)
            card.piece1 = CORAL_PURPLE;
            card.piece2 = CORAL_PURPLE;
            card.pattern = CreateSquare2x2Pattern(CORAL_ORANGE, 8);
            break;
            
        case 12: // Orange + Yellow -> height-3 orange (5 points)
            card.piece1 = CORAL_ORANGE;
            card.piece2 = CORAL_YELLOW;
            card.pattern = CreateHeight3Pattern(CORAL_ORANGE, 5);
            break;
            
        case 13: // Purple pieces -> 3 purple line (5 points)
            card.piece1 = CORAL_PURPLE;
            card.piece2 = CORAL_PURPLE;
            card.pattern = CreateLine3Pattern(CORAL_PURPLE, 5);
            break;
            
        case 14: // Orange pieces -> 3 green line (4 points)
            card.piece1 = CORAL_ORANGE;
            card.piece2 = CORAL_ORANGE;
            card.pattern = CreateLine3Pattern(CORAL_GREEN, 4);
            break;
            
        default:
            // Fallback pattern
            card.piece1 = colors[cardIndex % 4];
            card.piece2 = colors[(cardIndex + 1) % 4];
            card.pattern = CreateLine2Pattern(colors[(cardIndex + 2) % 4], 2);
            break;
    }
    
    return card;
}

void CardsInitAndShuffle(GameState* g)
{
    g->deckSize = DECK_MAX;

    // Create realistic Reef cards with proper scoring patterns
    for (int i = 0; i < DECK_MAX; ++i) {
        g->deck[i] = CreateReefCard(i);
    }

    // Seed once; keep simple and deterministic-ish if desired
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    ShuffleDeckInternal(g->deck, g->deckSize);
}

void DisplayInit(GameState* g)
{
    for (int i = 0; i < CARD_DISPLAY_SIZE; ++i) {
        g->displayTokens[i] = 0;
        DisplayRefillSlot(g, i);
    }
}

void DisplayRefillSlot(GameState* g, int index)
{
    if (g->deckSize <= 0) return;
    if (index < 0 || index >= CARD_DISPLAY_SIZE) return;

    g->display[index] = g->deck[g->deckSize - 1];
    g->deckSize--;
}

void DealInitialHands(GameState* g)
{
    for (int p = 0; p < g->playersCount; ++p) {
        g->players[p].handSize = 0;
        for (int i = 0; i < 2; ++i) {
            if (g->deckSize <= 0) break;
            g->players[p].hand[g->players[p].handSize] = g->deck[g->deckSize - 1];
            g->players[p].handSize++;
            g->deckSize--;
        }
    }
}

int FindDisplayLowestPointsIndex(const GameState* g)
{
    int bestIndex = 0;
    int bestPoints = g->display[0].pattern.pointValue;
    for (int i = 1; i < CARD_DISPLAY_SIZE; ++i) {
        if (g->display[i].pattern.pointValue < bestPoints) {
            bestPoints = g->display[i].pattern.pointValue;
            bestIndex = i;
        }
    }
    return bestIndex;
}
