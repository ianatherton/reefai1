#include "constants.h"
#include <stddef.h>

const int SUPPLY_PER_COLOR_2P = 18;
const int INITIAL_POINTS = 3;

const Color CORAL_COLOR_MAP[5] = {
    (Color){ 180, 180, 180, 255 }, // NONE -> GRAY
    YELLOW,
    ORANGE,
    PURPLE,
    GREEN
};

// Assets: base path and filenames (resources may be missing; UI falls back when not found)
const char* ASSET_PATH = "resources/graphics/";

const char* TEX_CORAL_FILE[5] = {
    NULL,                 // index 0 unused
    "coral_yellow.png",
    "coral_red.png",      // using red art for orange color
    "coral_indigo.png",   // using indigo art for purple color
    "coral_green.png"
};

const char* TEX_CARD_BG_FILE    = "card_bg.png";
const char* TEX_DECK_BACK_FILE  = "deck_back.png";
const char* TEX_BOARD_CELL_FILE = "board_cell.png";
const char* TEX_TOKEN_FILE      = "point_token_1.png"; // use 1-point token as generic token icon

const char* CORAL_COLOR_NAME[5] = {
    "None",
    "Yellow",
    "Orange",
    "Purple",
    "Green"
};
