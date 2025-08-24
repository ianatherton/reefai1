#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>
#include <stdint.h>
#include "raylib.h"

// Dimensions and limits
enum 
{
    SCREEN_WIDTH  = 1280,  // 720p width
    SCREEN_HEIGHT = 720,   // 720p height

    BOARD_SIZE        = 4,
    MAX_STACK_HEIGHT  = 4,
    MAX_HAND_SIZE     = 4,
    CARD_DISPLAY_SIZE = 3,
    DECK_MAX          = 60,

    PLAYERS_MAX = 2
};

// Coral colors
typedef enum 
{
    CORAL_NONE   = 0,
    CORAL_YELLOW = 1,
    CORAL_ORANGE = 2,
    CORAL_PURPLE = 3,
    CORAL_GREEN  = 4
} CoralColor;

// Board cell stack
typedef struct {
    CoralColor pieces[MAX_STACK_HEIGHT];
    int height;
} CoralStack;

// Pattern types for scoring
typedef enum {
    PATTERN_NONE = 0,
    PATTERN_LINE_2,      // 2 pieces in a line
    PATTERN_LINE_3,      // 3 pieces in a line
    PATTERN_SQUARE_2X2,  // 2x2 square
    PATTERN_L_SHAPE,     // L-shaped pattern
    PATTERN_HEIGHT_2,    // Exactly 2 high
    PATTERN_HEIGHT_3,    // Exactly 3 high
    PATTERN_HEIGHT_4,    // Exactly 4 high
    PATTERN_HEIGHT_2_PLUS, // 2+ high
    PATTERN_HEIGHT_3_PLUS, // 3+ high
    PATTERN_MIXED_COLORS,  // Multiple specific colors
    PATTERN_ADJACENCY      // Adjacent pieces pattern
} PatternType;

// Pattern cell for scoring requirements
typedef struct {
    CoralColor color;    // Required color (CORAL_NONE = any color)
    int minHeight;       // Minimum height (0 = any height)
    int exactHeight;     // Exact height required (0 = any height)
    bool isWild;         // True if any color is acceptable
} PatternCell;

// Scoring pattern definition
typedef struct {
    PatternType type;
    PatternCell cells[4][4];  // 4x4 grid for pattern matching
    int width, height;        // Actual pattern dimensions
    int pointValue;           // Points awarded per match
} ScoringPattern;

// Card with both coral pieces and scoring pattern
typedef struct 
{
    CoralColor piece1;
    CoralColor piece2;
    ScoringPattern pattern;
} Card;

// Player
typedef struct 
{
    CoralStack board[BOARD_SIZE][BOARD_SIZE];
    Card hand[MAX_HAND_SIZE];
    int handSize;
    int points;
    int id;
} Player;

// Placement state for manual coral placement
typedef struct {
    bool active;                    // Whether we're in placement mode
    CoralColor piecesToPlace[2];    // The two coral pieces to place
    int piecesPlaced;              // How many pieces have been placed (0, 1, or 2)
    int cardPoints;                // Points from the played card
    ScoringPattern scoringPattern;  // Pattern to score after placement
} PlacementState;

// Game state
typedef struct 
            {
    int playersCount;
    Player players[PLAYERS_MAX];

    Card deck[DECK_MAX];
    int deckSize;

    Card display[CARD_DISPLAY_SIZE];
    int displayTokens[CARD_DISPLAY_SIZE];

    int supplies[5]; // index by CoralColor (0 unused)

    int currentPlayer;
    bool gameEnded;
    
    PlacementState placement;       // Manual placement state
} GameState;

// Shared constants
extern const int SUPPLY_PER_COLOR_2P;
extern const int INITIAL_POINTS;

extern const Color CORAL_COLOR_MAP[5];
extern const char* CORAL_COLOR_NAME[5];

// Assets (paths and filenames)
extern const char* ASSET_PATH;                 // e.g., "resources/graphics/"
extern const char* TEX_CORAL_FILE[5];          // index by CoralColor (0 unused)
extern const char* TEX_CARD_BG_FILE;           // e.g., "card_bg.png"
extern const char* TEX_DECK_BACK_FILE;         // e.g., "deck_back.png"
extern const char* TEX_BOARD_CELL_FILE;        // e.g., "board_cell.png"
extern const char* TEX_TOKEN_FILE;             // e.g., "token.png"
extern const char* TEX_GAMEBOARD_FILE;         // e.g., "gameboard.png"
extern const char* FONT_PATH;                  // e.g., "resources/fonts/"
extern const char* FONT_FILE;                  // e.g., "Lexend-Bold.ttf"

// UI layout - Scaled down 62.5% for 720p display (25% smaller than before)
enum                        {
    UI_CELL_SIZE   = 96,      // Each cell is 96x96 (75% of 128x128)
    UI_BOARD_SIZE  = 384,     // Total board is 384x384 (4x4 * 96)
    UI_BOARD1_X    = 20,      // Player 1 board on left
    UI_BOARD1_Y    = 80,
    UI_BOARD2_X    = 500,     // Player 2 board on right (adjusted for smaller boards)
    UI_BOARD2_Y    = 80,

    UI_CARD_W      = 80,      // Scaled down cards
    UI_CARD_H      = 110,
    UI_CARD_GAP    = 8,

    UI_MARKET_X    = 20,      // Market below boards
    UI_MARKET_Y    = 610,

    // Deck position (same row as market)
    UI_DECK_X      = UI_MARKET_X + (CARD_DISPLAY_SIZE * (UI_CARD_W + UI_CARD_GAP)) + 20,
    UI_DECK_Y      = UI_MARKET_Y,

    UI_HAND1_X     = 20,      // Player 1 hand under their board
    UI_HAND1_Y     = 480,
    UI_HAND2_X     = 500,     // Player 2 hand under their board
    UI_HAND2_Y     = 480,

    UI_SUPPLY_X    = 950,     // Supplies on far right
    UI_SUPPLY_Y    = 80
};

#endif
