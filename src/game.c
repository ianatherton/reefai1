#include "game.h"
#include "cards.h"
#include "ui.h"
#include "assets.h"

// Forward declarations
static void NextPlayer(GameState* g);
static void CheckEnd(GameState* g);

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

static bool PlaceCoralAt(GameState* g, Player* p, CoralColor color, int row, int col)
{
    if (color == CORAL_NONE) return false;
    if (g->supplies[color] <= 0) return false;
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return false;
    
    CoralStack* s = &p->board[row][col];
    if (s->height >= MAX_STACK_HEIGHT) return false;
    
    s->pieces[s->height] = color;
    s->height++;
    g->supplies[color]--;
    return true;
}

static void StartPlacement(GameState* g, Card card)
{
    g->placement.active = true;
    g->placement.piecesToPlace[0] = card.piece1;
    g->placement.piecesToPlace[1] = card.piece2;
    g->placement.piecesPlaced = 0;
    g->placement.cardPoints = card.points;
}

static void FinishPlacement(GameState* g)
{
    if (g->placement.active && g->placement.piecesPlaced == 2) {
        // Award points and end turn
        g->players[g->currentPlayer].points += g->placement.cardPoints;
        g->placement.active = false;
        CheckEnd(g);
        if (!g->gameEnded) {
            NextPlayer(g);
        }
    }
}

static bool HandleMousePlacement(GameState* g)
{
    if (!g->placement.active) return false;
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return false;
    
    Vector2 mousePos = GetMousePosition();
    Player* currentPlayer = &g->players[g->currentPlayer];
    
    // Determine which board was clicked based on player
    int boardX = (g->currentPlayer == 0) ? UI_BOARD1_X : UI_BOARD2_X;
    int boardY = (g->currentPlayer == 0) ? UI_BOARD1_Y : UI_BOARD2_Y;
    
    // Check if click is within current player's board
    if (mousePos.x >= boardX && mousePos.x < boardX + UI_BOARD_SIZE &&
        mousePos.y >= boardY && mousePos.y < boardY + UI_BOARD_SIZE) {
        
        // Calculate which cell was clicked
        int col = (int)((mousePos.x - boardX) / UI_CELL_SIZE);
        int row = (int)((mousePos.y - boardY) / UI_CELL_SIZE);
        
        // Try to place the current coral piece
        CoralColor colorToPlace = g->placement.piecesToPlace[g->placement.piecesPlaced];
        if (PlaceCoralAt(g, currentPlayer, colorToPlace, row, col)) {
            g->placement.piecesPlaced++;
            if (g->placement.piecesPlaced == 2) {
                FinishPlacement(g);
            }
            return true;
        }
    }
    return false;
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
    
    // Initialize placement state
    g->placement.active = false;
    g->placement.piecesPlaced = 0;
    g->placement.cardPoints = 0;

    InitSupplies(g);
    InitPlayers(g);

    CardsInitAndShuffle(g);
    DisplayInit(g);
    DealInitialHands(g);
}

void GameUpdate(GameState* g)
{
    if (g->gameEnded) return;

    // Handle mouse placement if in placement mode
    if (g->placement.active) {
        HandleMousePlacement(g);
        return; // Don't process other inputs during placement
    }

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

    // Play from hand [Q,W,E,R] -> start manual placement mode
    if (!action) {
        int playIndex = -1;
        if (IsKeyPressed(KEY_Q) && pl->handSize >= 1) playIndex = 0;
        else if (IsKeyPressed(KEY_W) && pl->handSize >= 2) playIndex = 1;
        else if (IsKeyPressed(KEY_E) && pl->handSize >= 3) playIndex = 2;
        else if (IsKeyPressed(KEY_R) && pl->handSize >= 4) playIndex = 3;

        if (playIndex >= 0) {
            Card card = pl->hand[playIndex];
            // Remove card from hand
            for (int i = playIndex; i < pl->handSize - 1; ++i) {
                pl->hand[i] = pl->hand[i + 1];
            }
            pl->handSize--;

            // Start manual placement mode
            StartPlacement(g, card);
            action = true;
        }
    }

    if (action && !g->placement.active) {
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

    // Determine if we should highlight valid positions and which color to preview
    bool highlightPlayer1 = g->placement.active && g->currentPlayer == 0;
    bool highlightPlayer2 = g->placement.active && g->currentPlayer == 1;
    CoralColor previewColor = g->placement.active ? 
        g->placement.piecesToPlace[g->placement.piecesPlaced] : CORAL_NONE;

    UI_DrawPlayerBoard(&g->players[0], UI_BOARD1_X, UI_BOARD1_Y, highlightPlayer1, previewColor);
    UI_DrawPlayerBoard(&g->players[1], UI_BOARD2_X, UI_BOARD2_Y, highlightPlayer2, previewColor);

    UI_DrawMarket(g);
    UI_DrawDeck(g);

    // Draw both players' hands
    UI_DrawHand(&g->players[0], UI_HAND1_X, UI_HAND1_Y, g->currentPlayer == 0 ? -1 : -2);
    UI_DrawHand(&g->players[1], UI_HAND2_X, UI_HAND2_Y, g->currentPlayer == 1 ? -1 : -2);

    UI_DrawSupplies(g);
}
