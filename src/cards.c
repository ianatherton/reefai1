#include <stdlib.h>
#include <time.h>
#include "cards.h"

static void ShuffleDeckInternal(Card* deck, int n)
{
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Card tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

void CardsInitAndShuffle(GameState* g)
{
    g->deckSize = DECK_MAX;

    // Simple, readable deck seed: cycle colors; base points vary 2..6
    for (int i = 0; i < DECK_MAX; ++i) {
        CoralColor c1 = (CoralColor)((i % 4) + 1);
        CoralColor c2 = (CoralColor)(((i + 1) % 4) + 1);
        int pts = 2 + (i % 5); // 2..6

        g->deck[i].piece1 = c1;
        g->deck[i].piece2 = c2;
        g->deck[i].points = pts;
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
    int bestPoints = g->display[0].points;
    for (int i = 1; i < CARD_DISPLAY_SIZE; ++i) {
        if (g->display[i].points < bestPoints) {
            bestPoints = g->display[i].points;
            bestIndex = i;
        }
    }
    return bestIndex;
}
