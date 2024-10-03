CC = gcc
OUTPUT = Test

all: SStr.o tests.o
	@${CC} tests.o SStr.o -o ${OUTPUT}
	@rm ./*.o

tests.o:
	@${CC} -c tests.c

SStr.o:
	@${CC} -c ../SStr.c
