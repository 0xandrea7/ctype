#include "game.h"
#include "screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int handleUserInput(game *g, dict *d){
    if (g->inputChar == 127 || g->inputChar == '\b') {
        if (g->currentChar > 0) {
            g->currentChar--;
            g->userInput[g->currentWord][g->currentChar] = '\0'; 
        }
        return 0;
    }

    if (g->inputChar == ' ') {
        int randomWordIdx = g->randomNumbers[g->currentWord];
        int wordLen = strlen(d->words[randomWordIdx]);

        if (g->currentChar < wordLen) {
            int skippedChars = wordLen - g->currentChar;
            g->errors[g->currentWord] += skippedChars;
            g->errorCounter += skippedChars;
        }

        g->currentWord++;
        g->currentChar = 0;

        if (g->currentWord >= g->totalWord) return -1;
        return 0;
    }

    int randomWordIdx = g->randomNumbers[g->currentWord];
    int wordLen = strlen(d->words[randomWordIdx]);

    if (g->currentChar < wordLen) {
        g->userInput[g->currentWord][g->currentChar] = g->inputChar;
        
        if (d->words[randomWordIdx][g->currentChar] != g->inputChar) {
            g->errors[g->currentWord]++;
            g->errorCounter++;
        }
        
        g->currentChar++;

        if (g->currentWord == g->totalWord - 1 && g->currentChar == wordLen) {
            g->currentWord++;
            return -1;
        }
    }
    return 0;
}

void printStats(screen *s, game *g, cursor *c, timer *t){
    screenAppend(s, "\x1b[?25l", 6);
    screenAppend(s, "\x1b[H", 3);

    printLogo(s, c);

    char buffTimer[256];
    char buffWord[256];
    char buffError[256];
    char buffWpm[256];
    char buffWpmRaw[256];

    int correctWords = g->currentWord;
    for (int i = 0; i < g->currentWord; i++) {
        if (g->errors[i] != 0) correctWords--; 
    }

    if (t->elapsed > t->seconds) {
        t->elapsed = t->seconds; 
    }

    double minutes = t->elapsed / 60.0;

    double finalWpm = correctWords / minutes;
    double finalRawWpm = g->currentWord / minutes;

    int buffLenTimer = snprintf(buffTimer, sizeof(buffTimer), "          time : %s%.2f%s\r\n\r\n", TH_TIMER, t->elapsed, TH_RESET);
    int buffLenWord = snprintf(buffWord, sizeof(buffWord), "          word : %s%d / %d %s\r\n\r\n", TH_CURSOR, g->currentWord, g->totalWord, TH_RESET);

    int buffLenError = snprintf(buffError, sizeof(buffError), "          errors : %s%d%s\r\n\r\n", TH_ERROR, g->errorCounter, TH_RESET);

    int buffWpmLen = snprintf(buffWpm, sizeof(buffWpm), "          wpm : %s%.2f%s\r\n\r\n", TH_CURSOR, finalWpm, TH_RESET);
    int buffWpmRawLen = snprintf(buffWpmRaw, sizeof(buffWpmRaw), "          raw wpm : %s%.2f%s\r\n\r\n", TH_CURSOR, finalRawWpm, TH_RESET);

    char exit[] = "          press Esc to exit\n";
    int exitLen = sizeof(exit);

    screenAppend(s, buffTimer, buffLenTimer);
    screenAppend(s, buffWord, buffLenWord);
    screenAppend(s, buffError, buffLenError);
    screenAppend(s, buffWpm, buffWpmLen);
    screenAppend(s, buffWpmRaw, buffWpmRawLen);
    screenAppend(s, exit, exitLen+1);

    if (s->buff != NULL && s->len > 0) {
        write(STDOUT_FILENO, s->buff, s->len);
        free(s->buff);
        s->buff = NULL;
        s->len = 0;
    }
}

void startGame(screen *s, game *g, dict *d, cursor *c, timer *t){
    initTimer(t);

    while (1){
        clock_gettime(CLOCK_MONOTONIC, &t->currentTime);
        updateTimer(t);

        if (t->remaining <= 0) {
            break;
        }

        refreshScreen(s, g, d, c, t);

        if (read(STDIN_FILENO, &g->inputChar, 1) > 0) {
            if (g->inputChar == 27 || g->inputChar == 3) break;
            if (handleUserInput(g, d) == -1) break;
        }
    }
}
