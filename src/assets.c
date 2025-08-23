#include "assets.h"
#include <string.h>
#include <stdio.h>

Assets gAssets; // zero-init by C static storage

static bool LoadTextureIfExists(Texture2D* outTex, bool* outFlag, const char* file)
{
    if (file == NULL) { *outFlag = false; return false; }

    char path[512];
    snprintf(path, sizeof(path), "%s%s", ASSET_PATH, file);

    if (FileExists(path)) {
        *outTex = LoadTexture(path);
        *outFlag = (*outTex).id != 0;
        return *outFlag;
    }
    *outFlag = false;
    return false;
}

static bool LoadFontIfExists(Font* outFont, bool* outFlag, const char* basePath, const char* file, int fontSize)
{
    if (file == NULL) { *outFlag = false; return false; }

    char path[512];
    snprintf(path, sizeof(path), "%s%s", basePath, file);

    if (FileExists(path)) {
        *outFont = LoadFontEx(path, fontSize, 0, 0);
        *outFlag = (*outFont).texture.id != 0;
        return *outFlag;
    }
    *outFlag = false;
    return false;
}

void AssetsLoadAll(void)
{
    // Coral textures (index 0 unused)
    for (int i = 0; i < 5; ++i) {
        gAssets.coralLoaded[i] = false;
    }
    for (int i = 1; i <= 4; ++i) {
        LoadTextureIfExists(&gAssets.coral[i], &gAssets.coralLoaded[i], TEX_CORAL_FILE[i]);
    }

    // Other textures
    gAssets.cardBgLoaded   = false;
    gAssets.deckBackLoaded = false;
    gAssets.boardCellLoaded= false;
    gAssets.tokenLoaded    = false;
    gAssets.gameboardLoaded= false;
    gAssets.fontLoaded     = false;

    LoadTextureIfExists(&gAssets.cardBg,   &gAssets.cardBgLoaded,   TEX_CARD_BG_FILE);
    LoadTextureIfExists(&gAssets.deckBack, &gAssets.deckBackLoaded, TEX_DECK_BACK_FILE);
    LoadTextureIfExists(&gAssets.boardCell,&gAssets.boardCellLoaded,TEX_BOARD_CELL_FILE);
    LoadTextureIfExists(&gAssets.token,    &gAssets.tokenLoaded,    TEX_TOKEN_FILE);
    LoadTextureIfExists(&gAssets.gameboard,&gAssets.gameboardLoaded,TEX_GAMEBOARD_FILE);
    
    // Load custom font at base size 32 (will be scaled as needed)
    LoadFontIfExists(&gAssets.customFont, &gAssets.fontLoaded, FONT_PATH, FONT_FILE, 32);
}

void AssetsUnloadAll(void)
{
    for (int i = 1; i <= 4; ++i) {
        if (gAssets.coralLoaded[i]) {
            UnloadTexture(gAssets.coral[i]);
            gAssets.coralLoaded[i] = false;
        }
    }

    if (gAssets.cardBgLoaded)   { UnloadTexture(gAssets.cardBg);     gAssets.cardBgLoaded = false; }
    if (gAssets.deckBackLoaded) { UnloadTexture(gAssets.deckBack);   gAssets.deckBackLoaded = false; }
    if (gAssets.boardCellLoaded){ UnloadTexture(gAssets.boardCell);  gAssets.boardCellLoaded = false; }
    if (gAssets.tokenLoaded)    { UnloadTexture(gAssets.token);      gAssets.tokenLoaded = false; }
    if (gAssets.gameboardLoaded){ UnloadTexture(gAssets.gameboard);  gAssets.gameboardLoaded = false; }
    if (gAssets.fontLoaded)     { UnloadFont(gAssets.customFont);    gAssets.fontLoaded = false; }
}
