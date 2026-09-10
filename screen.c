#include "screen.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printWords(dict *d){
    for (int i = 0; i < d->lines; i++) {
        printf("%s\n", d->words[i]);
    }
}

void printLogo(screen *s, cursor *c){
    screenAppend(s, "\r\n", 2);
    screenAppend(s, TH_LOGO, sizeof(TH_LOGO) - 1);

    int logoWidth = 27;
    int pad = (c->maxX - logoWidth) / 2;
    if (pad < 0) pad = 0;

    const char *const lines[] = {
        "   ___ _______   _____ ___ ",
        "  / __|_   _\\ \\ / / _ \\ __|",
        " | (__  | |  \\ V /|  _/ _| ",
        "  \\___| |_|   |_| |_| |___|"

        /*

        " ██████╗ ██████ ██    ██╗ ███████  ███████╗",
        "██╔════╝ ██╔═██ ╚██╗ ██╔╝ ██╔══██╗ ██╔════╝",
        "██║      ██████  ╚████╔╝  ██████╔╝ █████╗  ",
        "██║      ██║      ╚██╔╝   ██╔═══╝  ██╔══╝  ",
        "╚██████╗ ██║       ██║    ██║      ███████╗",
        " ╚═════╝ ╚═╝       ╚═╝    ╚═╝      ╚══════╝"
        */
    };
    int numLines = sizeof(lines) / sizeof(lines[0]);
    char padBuff[128];
    if (pad > 0 && pad < (int)sizeof(padBuff)) {
        memset(padBuff, ' ', pad);
    }
    screenAppend(s, "\033[95m", 5);
    
    for (int i = 0; i < numLines; i++) {
        if (pad > 0) {
            screenAppend(s, padBuff, pad);
        }
        screenAppend(s, lines[i], strlen(lines[i]));
        screenAppend(s, "\r\n", 2); 
    }
    screenAppend(s, TH_RESET, sizeof(TH_RESET) - 1);
    screenAppend(s, "\r\n\r\n", 4); 
}

void screenAppend(screen *s, const char *str, int len) {
    char *new_buff = realloc(s->buff, s->len + len);
    if (new_buff == NULL) return;
    
    memcpy(&new_buff[s->len], str, len);
    
    s->buff = new_buff; 
    s->len += len;
}

void refreshScreen(screen *s, game *g, dict *d, cursor *c, timer *t) {
    if (s->buff != NULL) {
        free(s->buff);
        s->buff = NULL;
    }
    s->len = 0;

    screenAppend(s, "\x1b[?25l", 6);
    screenAppend(s, "\x1b[H", 3); 

    printLogo(s, c);

    char timerBuf[64];
    double timeToShow = (t->remaining > 0) ? t->remaining : 0;
    
    int tLen = snprintf(timerBuf, sizeof(timerBuf), "          time : %s%.2f%s\r\n\r\n", TH_TIMER, timeToShow, TH_RESET);
    screenAppend(s, timerBuf, tLen);

    int margin = 10;
    int currentX = margin;

    screenAppend(s, "          ", margin); 

    int pageSize = 100;
    int startWordIdx = (g->currentWord / pageSize) * pageSize;

    int endWordIdx = startWordIdx + pageSize;
    if (endWordIdx > g->totalWord) endWordIdx = g->totalWord;
    
    for (int i = startWordIdx; i < endWordIdx; i++) {
        int randomWordIdx = g->randomNumbers[i];
        int wordLen = strlen(d->words[randomWordIdx]);

        if (currentX + wordLen >= c->maxX - margin) {
            screenAppend(s, "\r\n          ", margin + 2); 
            currentX = margin; 
        }
        
        for (int j = 0; j < wordLen; j++){
            if (i == g->currentWord && j == g->currentChar) {
                screenAppend(s, TH_CURSOR, sizeof(TH_CURSOR) - 1);
            }
            else{
                char typedChar = g->userInput[i][j];
                if (typedChar == '\0') {
                    screenAppend(s, TH_UNTYPED, sizeof(TH_UNTYPED) - 1);
                } else if (typedChar == d->words[randomWordIdx][j]) {
                    screenAppend(s, TH_CORRECT, sizeof(TH_CORRECT) - 1);
                } else {
                    screenAppend(s, TH_ERROR, sizeof(TH_ERROR) - 1);
                }
            }
            screenAppend(s, &d->words[randomWordIdx][j], 1);
            currentX++;
        }

        if (i == g->currentWord && g->currentChar == wordLen) {
            screenAppend(s, TH_CURSOR_BG, sizeof(TH_CURSOR_BG) - 1);
            screenAppend(s, " ", 1);
            screenAppend(s, TH_RESET, sizeof(TH_RESET) - 1);
        } else {
            screenAppend(s, TH_RESET, sizeof(TH_RESET) - 1);
            screenAppend(s, " ", 1);
        }
        currentX++;
    }
    screenAppend(s, "\x1b[K", 3);

    if (s->buff != NULL && s->len > 0) {
        write(STDOUT_FILENO, s->buff, s->len);
        free(s->buff);
        s->buff = NULL;
        s->len = 0;
    }
}

void initTimer(timer *t){
    clock_gettime(CLOCK_MONOTONIC, &t->startTime);
}

void updateTimer(timer *t){
    t->elapsed = (t->currentTime.tv_sec - t->startTime.tv_sec) +  (t->currentTime.tv_nsec - t->startTime.tv_nsec) / 1e9;
    t->remaining = t->seconds - t->elapsed;
}

void clearScreen() {
    write(STDOUT_FILENO, "\x1b[2J\x1b[H\x1b[?25h", 13);
}

