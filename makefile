CC=gcc
AS=as
opts=-Wall -O3

bin=bin
obj=obj
src=src

main: main.o cpu.o mem.o video.o stepper.o
	${CC} ${opts} ${obj}/main.o ${obj}/cpu.o ${obj}/mem.o ${obj}/video.o ${obj}/stepper.o -lSDL2 -o ${bin}/main

main.o: ${src}/main.c
	${CC} ${opts} ${src}/main.c -c -o ${obj}/main.o

cpu.o: ${src}/cpu.c
	${CC} ${opts} ${src}/cpu.c -c -o ${obj}/cpu.o

mem.o: ${src}/mem.c
	${CC} ${opts} ${src}/mem.c -c -o ${obj}/mem.o

video.o: ${src}/video.c
	${CC} ${opts} ${src}/video.c -c -o ${obj}/video.o

stepper.o: ${src}/stepper.c
	${CC} ${opts} ${src}/stepper.c -c -o ${obj}/stepper.o

# carry.o: carry.s
# 	${AS} carry.s -o carry.o

# instr.o: instr.s
# 	${AS} instr.s -o instr.o