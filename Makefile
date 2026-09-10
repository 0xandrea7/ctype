ctyper: main.c game.c screen.c init.c
	gcc -Wall -Wextra -O2 main.c game.c screen.c init.c -o ctyper

clean:
	rm -f ctyper

