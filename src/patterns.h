#ifndef PATTERNS_H
#define PATTERNS_H

#include "constants.h"

// Pattern matching functions
int ScorePattern(const Player* player, const ScoringPattern* pattern);
bool MatchesPatternAt(const Player* player, const ScoringPattern* pattern, int startRow, int startCol);
ScoringPattern CreateSimplePattern(PatternType type, CoralColor color, int points);
ScoringPattern CreateHeightPattern(PatternType type, CoralColor color, int height, int points);

// Predefined pattern creators for common card types
ScoringPattern CreateLine2Pattern(CoralColor color, int points);
ScoringPattern CreateLine3Pattern(CoralColor color, int points);
ScoringPattern CreateSquare2x2Pattern(CoralColor color, int points);
ScoringPattern CreateLShapePattern(CoralColor color, int points);
ScoringPattern CreateHeight2Pattern(CoralColor color, int points);
ScoringPattern CreateHeight3Pattern(CoralColor color, int points);

#endif
