#ifndef GAME_H
#define GAME_H

#include <stdio.h>

typedef struct Dict {
    int maxLen;
    int lines;
    int totalLines;
    char **words;
} dict;

typedef struct Game {
    int currentWord;
    int totalWord;
    int *randomNumbers;

    char inputChar;
    int currentChar;  
    char **userInput;

    int *errors;
    int errorCounter;
} game;

struct Screen;
struct Timer;
struct Cursor;

int handleUserInput(game *g, dict *d);

void startGame(struct Screen *s, game *g, dict *d, struct Cursor *c, struct Timer *t);

void printStats(struct Screen *s, game *g, struct Cursor *c, struct Timer *t);

#endif
