#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "screen.h"
#include "game.h"
#include "init.h"

void usage(char *binaryName){
    printf("Usage : %s <dictionary path>\n", binaryName);
    return;
}

int main(int argc, char *argv[]){
    clearScreen();
    srand((unsigned int) time(NULL));
    
    dict d;
    game g;
    screen s;
    s.buff = NULL;
    cursor c;
    timer t;

    char *filepath;
    int filepathArg = 1;
    int secondsArg = 1;
    int wordsArg = 1;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-w") == 0) {
            if (argv[i+1] != NULL && wordsArg) {
                int temp = atoi(argv[i+1]);
                if (temp >= 1 && temp <= 10000) {
                    g.totalWord = temp;
                    wordsArg = 0;
                }
            }
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (argv[i+1] != NULL && secondsArg) {
                int temp = atoi(argv[i+1]);
                if (temp >= 0 && temp <= 10000) {
                    t.seconds = temp;
                    secondsArg = 0;
                }
            }
        }
        else if (strcmp(argv[i], "-f") == 0) {
            if (argv[i+1] != NULL && filepathArg) {
                filepath = argv[i+1];
                filepathArg = 0;
            }
        }
    }

    if (filepathArg) filepath = "top_1000_english";
    if (secondsArg) t.seconds = 60;
    if (wordsArg) g.totalWord = 100;

    if (initDict(&d, filepath) == -1) return 1;
    if (initGame(&g, &d) == -1) {
        freeDict(&d);
        return 1;
    }
    if (initScreen(&s) == -1) {
        freeDict(&d);
        freeGame(&g);
        freeScreen(&s);
        return -1;
    }
    if (initCursor(&c) == -1) {
        freeDict(&d);
        freeGame(&g);
        freeScreen(&s);
        return -1;
    }

    enableRawMode(); 

    startGame(&s, &g, &d, &c, &t);

    clearScreen();
    printStats(&s, &g, &c, &t);

    while (1){
         if (read(STDIN_FILENO, &g.inputChar, 1) > 0) {
            if (g.inputChar == 27 || g.inputChar == 3) break;
         }
    }

    clearScreen();

    disableRawMode();

    freeDict(&d);
    freeGame(&g);
    freeScreen(&s);
    return 0;
}
