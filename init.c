#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "init.h"
#include "game.h"
#include "screen.h"

int initDict(dict *d, char *filename){
    FILE *fpnt = fopen(filename, "r");
    if (fpnt == NULL) {
        printf("Cannot open file %s\n", filename);
        return -1;
    }

    d->maxLen = 150;

    d->lines = 0;
    d->totalLines = 1000;

    d->words = (char **) malloc(d->totalLines * sizeof(char *));
    if (d->words == NULL) {
        fclose(fpnt);
        return -1;
   }

    char buff[d->maxLen];

    while (fgets(buff, d->maxLen, fpnt) != NULL && d->lines < d->totalLines) {
        buff[strcspn(buff, "\n")] = '\0';

        d->words[d->lines] = (char *) malloc((strlen(buff) + 1) * sizeof(char));
        if (d->words[d->lines] == NULL) {
            fclose(fpnt);
            return -1;
        }

        strcpy(d->words[d->lines], buff);
        d->lines = d->lines + 1;
    }

    fclose(fpnt);
    return 0;
}

void freeDict(dict *d){
    for (int i = 0; i < d->lines; i++) {
        free(d->words[i]);
    }
    free(d->words);
}

int initGame(game *g, dict *d){
    g->currentChar = 0;
    g->currentWord = 0;
    // g->totalWord = d->lines;

    g->randomNumbers = (int *) malloc(g->totalWord * sizeof(int));
    if (g->randomNumbers == NULL) return -1;

    for (int i = 0; i < g->totalWord; i++) {
        g->randomNumbers[i] = rand() % d->lines;
    }

    g->userInput = (char **) malloc((g->totalWord) * sizeof(char *));
    if (g->userInput == NULL) return -1;

    for (int i = 0; i < g->totalWord; i++) {
        int randomWordIdx = g->randomNumbers[i];
        int wordLen = strlen(d->words[randomWordIdx]);

        g->userInput[i] = (char *) calloc(wordLen + 1, sizeof(char));
        if (g->userInput[i] == NULL) return -1;
    }

    g->errorCounter = 0;
    g->errors = (int *) malloc((g->totalWord) * sizeof(int));
    if (g->errors == NULL) return -1;
    for (int i = 0; i < g->totalWord; i++) g->errors[i] = 0;

    return 0;
}

void freeGame(game *g){
    if (g->randomNumbers != NULL) {
        free(g->randomNumbers);
    }

    if (g->userInput != NULL) {
        for (int i = 0; i < g->totalWord; i++) {
            if (g->userInput[i] != NULL) {
                free(g->userInput[i]);
            }
        }
        free(g->userInput);
    }

    if (g->errors != NULL) {
        free(g->errors);
    }
}

int initScreen(screen *s){
    s->len = 0;
    s->buff = NULL; 

    return 0;
}

void freeScreen(screen *s){
    if (s->buff != NULL) {
        free(s->buff);
    }
}

static struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios) == -1) {
        perror("tcsetattr fallita");
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        perror("tcgetattr fallita");
        exit(EXIT_FAILURE);
    }

    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr fallita");
        exit(EXIT_FAILURE);
    }
}

int initCursor(cursor *c){
    c->x = 0;
    c->y = 20;

	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
		c->maxY = w.ws_row;
		c->maxX = w.ws_col;
		return 0;
	}else {
		printf("Could not get dimension of the terminal\n");
		return -1;
	}
    return 0;
}

