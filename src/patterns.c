#include "patterns.h"
#include <string.h>

// Check if a cell matches the pattern requirements
static bool CellMatches(const CoralStack* stack, const PatternCell* cell) {
    if (stack->height == 0) return false;
    
    CoralColor topColor = stack->pieces[stack->height - 1];
    
    // Check color requirement
    if (!cell->isWild && cell->color != CORAL_NONE && cell->color != topColor) {
        return false;
    }
    
    // Check exact height requirement
    if (cell->exactHeight > 0 && stack->height != cell->exactHeight) {
        return false;
    }
    
    // Check minimum height requirement
    if (cell->minHeight > 0 && stack->height < cell->minHeight) {
        return false;
    }
    
    return true;
}

// Check if pattern matches at specific position
bool MatchesPatternAt(const Player* player, const ScoringPattern* pattern, int startRow, int startCol) {
    // Check bounds
    if (startRow + pattern->height > BOARD_SIZE || startCol + pattern->width > BOARD_SIZE) {
        return false;
    }
    
    // Check each cell in the pattern
    for (int r = 0; r < pattern->height; r++) {
        for (int c = 0; c < pattern->width; c++) {
            const PatternCell* cell = &pattern->cells[r][c];
            
            // Skip empty pattern cells
            if (cell->color == CORAL_NONE && cell->exactHeight == 0 && cell->minHeight == 0 && !cell->isWild) {
                continue;
            }
            
            const CoralStack* stack = &player->board[startRow + r][startCol + c];
            if (!CellMatches(stack, cell)) {
                return false;
            }
        }
    }
    
    return true;
}

// Score a pattern across the entire board
int ScorePattern(const Player* player, const ScoringPattern* pattern) {
    int matches = 0;
    bool used[BOARD_SIZE][BOARD_SIZE] = {false};
    
    // Try pattern at each position
    for (int r = 0; r <= BOARD_SIZE - pattern->height; r++) {
        for (int c = 0; c <= BOARD_SIZE - pattern->width; c++) {
            if (MatchesPatternAt(player, pattern, r, c)) {
                // Check if any cells are already used
                bool canUse = true;
                for (int pr = 0; pr < pattern->height && canUse; pr++) {
                    for (int pc = 0; pc < pattern->width && canUse; pc++) {
                        const PatternCell* cell = &pattern->cells[pr][pc];
                        if ((cell->color != CORAL_NONE || cell->exactHeight > 0 || cell->minHeight > 0 || cell->isWild) 
                            && used[r + pr][c + pc]) {
                            canUse = false;
                        }
                    }
                }
                
                if (canUse) {
                    // Mark cells as used
                    for (int pr = 0; pr < pattern->height; pr++) {
                        for (int pc = 0; pc < pattern->width; pc++) {
                            const PatternCell* cell = &pattern->cells[pr][pc];
                            if (cell->color != CORAL_NONE || cell->exactHeight > 0 || cell->minHeight > 0 || cell->isWild) {
                                used[r + pr][c + pc] = true;
                            }
                        }
                    }
                    matches++;
                }
            }
        }
    }
    
    return matches * pattern->pointValue;
}

// Create a 2-piece line pattern
ScoringPattern CreateLine2Pattern(CoralColor color, int points) {
    ScoringPattern pattern = {0};
    pattern.type = PATTERN_LINE_2;
    pattern.width = 2;
    pattern.height = 1;
    pattern.pointValue = points;
    
    pattern.cells[0][0].color = color;
    pattern.cells[0][1].color = color;
    
    return pattern;
}

// Create a 3-piece line pattern
ScoringPattern CreateLine3Pattern(CoralColor color, int points) {
    ScoringPattern pattern = {0};
    pattern.type = PATTERN_LINE_3;
    pattern.width = 3;
    pattern.height = 1;
    pattern.pointValue = points;
    
    pattern.cells[0][0].color = color;
    pattern.cells[0][1].color = color;
    pattern.cells[0][2].color = color;
    
    return pattern;
}

// Create a 2x2 square pattern
ScoringPattern CreateSquare2x2Pattern(CoralColor color, int points) {
    ScoringPattern pattern = {0};
    pattern.type = PATTERN_SQUARE_2X2;
    pattern.width = 2;
    pattern.height = 2;
    pattern.pointValue = points;
    
    pattern.cells[0][0].color = color;
    pattern.cells[0][1].color = color;
    pattern.cells[1][0].color = color;
    pattern.cells[1][1].color = color;
    
    return pattern;
}

// Create an L-shape pattern
ScoringPattern CreateLShapePattern(CoralColor color, int points) {
    ScoringPattern pattern = {0};
    pattern.type = PATTERN_L_SHAPE;
    pattern.width = 2;
    pattern.height = 2;
    pattern.pointValue = points;
    
    pattern.cells[0][0].color = color;
    pattern.cells[1][0].color = color;
    pattern.cells[1][1].color = color;
    
    return pattern;
}

// Create a height-2 pattern (exactly 2 pieces high)
ScoringPattern CreateHeight2Pattern(CoralColor color, int points) {
    ScoringPattern pattern = {0};
    pattern.type = PATTERN_HEIGHT_2;
    pattern.width = 1;
    pattern.height = 1;
    pattern.pointValue = points;
    
    pattern.cells[0][0].color = color;
    pattern.cells[0][0].exactHeight = 2;
    
    return pattern;
}

// Create a height-3 pattern (exactly 3 pieces high)
ScoringPattern CreateHeight3Pattern(CoralColor color, int points) {
    ScoringPattern pattern = {0};
    pattern.type = PATTERN_HEIGHT_3;
    pattern.width = 1;
    pattern.height = 1;
    pattern.pointValue = points;
    
    pattern.cells[0][0].color = color;
    pattern.cells[0][0].exactHeight = 3;
    
    return pattern;
}
