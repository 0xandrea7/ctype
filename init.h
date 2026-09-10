#ifndef INIT_H
#define INIT_H

#include "screen.h"

int initDict(dict *d, char *filename);
void freeDict(dict *d);

int initGame(game *g, dict *d);
void freeGame(game *g);

int initScreen(screen *s);
void freeScreen(screen *s);

int initCursor(cursor *c);

void disableRawMode();
void enableRawMode();

#endif
