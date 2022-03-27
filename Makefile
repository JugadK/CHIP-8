CC=gcc
CFLAGS=-I.

main:
	gcc -g src/main.c src/chip_keypad.c src/stack.c -lSDL2 -lGL -W 
