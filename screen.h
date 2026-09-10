#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>
#include "game.h"
#include <time.h>
#include <unistd.h>

#define TH_CORRECT    "\x1b[38;2;205;214;244m"
#define TH_UNTYPED    "\x1b[38;2;88;91;112m"   
#define TH_ERROR      "\x1b[38;2;243;139;168m" 
#define TH_CURSOR     "\x1b[38;2;166;227;161m" 
#define TH_CURSOR_BG  "\x1b[48;2;166;227;161m"
#define TH_LOGO       "\x1b[38;2;203;166;247m" 
#define TH_TIMER      "\x1b[38;2;249;226;175m"
#define TH_RESET      "\x1b[0m"                

typedef struct Screen {
    char *buff;
    size_t len;
} screen;

typedef struct Cursor {
    int maxX;
    int maxY;

    int x;
    int y;
} cursor;

typedef struct Timer {
    struct timespec startTime;
    struct timespec currentTime;

    int seconds;
    double remaining;
    double elapsed;
} timer; 

void printWords(dict *d);
void printLogo(screen *s, cursor *c);

void refreshScreen(screen *s, game *g, dict *d, cursor *c, timer *t);
void screenAppend(screen *s, const char *str, int len);

void initTimer(timer *t);
void updateTimer(timer *t);

void clearScreen();

#endif

