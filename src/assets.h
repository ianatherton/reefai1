#ifndef ASSETS_H
#define ASSETS_H

#include "constants.h"

typedef struct {
    Texture2D coral[5];
    bool coralLoaded[5];

    Texture2D cardBg;
    bool cardBgLoaded;

    Texture2D deckBack;
    bool deckBackLoaded;

    Texture2D boardCell;
    bool boardCellLoaded;

    Texture2D token;
    bool tokenLoaded;
} Assets;

extern Assets gAssets;

void AssetsLoadAll(void);
void AssetsUnloadAll(void);

#endif
