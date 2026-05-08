CC=gcc
opts=-Wall -O3

main: main.o cpu.o mem.o video.o
	${CC} ${opts} main.o cpu.o mem.o video.o -lSDL2 -o main

main.o: main.c
	${CC} ${opts} main.c -c -o main.o

cpu.o: cpu.c
	${CC} ${opts} cpu.c -c -o cpu.o

mem.o: mem.c
	${CC} ${opts} mem.c -c -o mem.o

video.o: video.c
	${CC} ${opts} video.c -c -o video.o