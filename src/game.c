#include "game.h"
#include "cards.h"
#include "ui.h"
#include "assets.h"

static void InitPlayers(GameState* g)
{
    g->playersCount = PLAYERS_MAX; // 2 for Phase 1

    for (int p = 0; p < g->playersCount; ++p) {
        Player* pl = &g->players[p];
        pl->id = p;
        pl->points = INITIAL_POINTS;
        pl->handSize = 0;

        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                pl->board[r][c].height = 0;
                for (int h = 0; h < MAX_STACK_HEIGHT; ++h) {
                    pl->board[r][c].pieces[h] = CORAL_NONE;
                }
            }
        }

        // Initial center four pieces (one of each color)
        pl->board[1][1].pieces[0] = CORAL_YELLOW; pl->board[1][1].height = 1;
        pl->board[1][2].pieces[0] = CORAL_ORANGE; pl->board[1][2].height = 1;
        pl->board[2][1].pieces[0] = CORAL_PURPLE; pl->board[2][1].height = 1;
        pl->board[2][2].pieces[0] = CORAL_GREEN;  pl->board[2][2].height = 1;
    }
}

static void InitSupplies(GameState* g)
{
    g->supplies[CORAL_NONE]   = 0;
    g->supplies[CORAL_YELLOW] = SUPPLY_PER_COLOR_2P;
    g->supplies[CORAL_ORANGE] = SUPPLY_PER_COLOR_2P;
    g->supplies[CORAL_PURPLE] = SUPPLY_PER_COLOR_2P;
    g->supplies[CORAL_GREEN]  = SUPPLY_PER_COLOR_2P;
}

static void AutoplacePiece(GameState* g, Player* p, CoralColor color)
{
    if (color == CORAL_NONE) return;
    if (g->supplies[color] <= 0) return;

    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            CoralStack* s = &p->board[r][c];
            if (s->height < MAX_STACK_HEIGHT) {
                s->pieces[s->height] = color;
                s->height++;
                g->supplies[color]--;
                return;
            }
        }
    }
}

static void NextPlayer(GameState* g)
{
    g->currentPlayer = (g->currentPlayer + 1) % g->playersCount;
}

static void CheckEnd(GameState* g)
{
    for (int i = 1; i <= 4; ++i) {
        if (g->supplies[i] <= 0) { g->gameEnded = true; return; }
    }
    if (g->deckSize <= 0) { g->gameEnded = true; }
}

void GameInit(GameState* g)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reef (Phase 1)");
    SetTargetFPS(60);
    AssetsLoadAll();

    g->gameEnded = false;
    g->currentPlayer = 0;

    InitSupplies(g);
    InitPlayers(g);

    CardsInitAndShuffle(g);
    DisplayInit(g);
    DealInitialHands(g);
}

void GameUpdate(GameState* g)
{
    if (g->gameEnded) return;

    Player* pl = &g->players[g->currentPlayer];
    bool action = false;

    // Take from market [1..3]
    if (!action) {
        if (IsKeyPressed(KEY_ONE)) {
            int idx = 0;
            if (pl->handSize < MAX_HAND_SIZE) {
                pl->points += g->displayTokens[idx];
                g->displayTokens[idx] = 0;
                pl->hand[pl->handSize++] = g->display[idx];
                DisplayRefillSlot(g, idx);
                action = true;
            }
        }
        else if (IsKeyPressed(KEY_TWO)) {
            int idx = 1;
            if (pl->handSize < MAX_HAND_SIZE) {
                pl->points += g->displayTokens[idx];
                g->displayTokens[idx] = 0;
                pl->hand[pl->handSize++] = g->display[idx];
                DisplayRefillSlot(g, idx);
                action = true;
            }
        }
        else if (IsKeyPressed(KEY_THREE)) {
            int idx = 2;
            if (pl->handSize < MAX_HAND_SIZE) {
                pl->points += g->displayTokens[idx];
                g->displayTokens[idx] = 0;
                pl->hand[pl->handSize++] = g->display[idx];
                DisplayRefillSlot(g, idx);
                action = true;
            }
        }
    }

    // Draw from deck [D], pay 1 point -> place on lowest display card
    if (!action && IsKeyPressed(KEY_D)) {
        if (pl->handSize < MAX_HAND_SIZE && g->deckSize > 0 && pl->points >= 1) {
            pl->points -= 1;
            int idx = FindDisplayLowestPointsIndex(g);
            g->displayTokens[idx] += 1;

            pl->hand[pl->handSize++] = g->deck[g->deckSize - 1];
            g->deckSize--;
            action = true;
        }
    }

    // Play from hand [Q,W,E,R] -> autoplace two pieces; score = card.points (Phase 1)
    if (!action) {
        int playIndex = -1;
        if (IsKeyPressed(KEY_Q) && pl->handSize >= 1) playIndex = 0;
        else if (IsKeyPressed(KEY_W) && pl->handSize >= 2) playIndex = 1;
        else if (IsKeyPressed(KEY_E) && pl->handSize >= 3) playIndex = 2;
        else if (IsKeyPressed(KEY_R) && pl->handSize >= 4) playIndex = 3;

        if (playIndex >= 0) {
            Card card = pl->hand[playIndex];
            for (int i = playIndex; i < pl->handSize - 1; ++i) {
                pl->hand[i] = pl->hand[i + 1];
            }
            pl->handSize--;

            AutoplacePiece(g, pl, card.piece1);
            AutoplacePiece(g, pl, card.piece2);

            // Placeholder scoring (Phase 1)
            pl->points += card.points;
            action = true;
        }
    }

    if (action) {
        CheckEnd(g);
        if (!g->gameEnded) {
            NextPlayer(g);
        }
    }
}

void GameDraw(const GameState* g)
{
    UI_DrawBackground();
    UI_DrawTopBar(g);

    UI_DrawPlayerBoard(&g->players[0], UI_BOARD1_X, UI_BOARD1_Y, false, CORAL_NONE);
    UI_DrawPlayerBoard(&g->players[1], UI_BOARD2_X, UI_BOARD2_Y, false, CORAL_NONE);

    UI_DrawMarket(g);
    UI_DrawDeck(g);

    // Draw both players' hands
    UI_DrawHand(&g->players[0], UI_HAND1_X, UI_HAND1_Y, g->currentPlayer == 0 ? -1 : -2);
    UI_DrawHand(&g->players[1], UI_HAND2_X, UI_HAND2_Y, g->currentPlayer == 1 ? -1 : -2);

    UI_DrawSupplies(g);
}
