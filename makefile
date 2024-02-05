main: nceffects.c
	gcc -o demo main.c nceffects.c minmax.c -lncurses -lm
