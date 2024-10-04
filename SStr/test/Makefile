CC = gcc
OUTPUT = Test
CFLAGS = -Wnonnull

all: SStr.o tests.o
	@${CC} tests.o SStr.o -o ${OUTPUT} ${CFLAGS}
	@rm ./*.o

tests.o:
	@${CC} -c tests.c

SStr.o:
	@${CC} -c ../SStr.c
