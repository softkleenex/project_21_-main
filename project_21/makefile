CC = gcc
CFLAGS = -Wall -Wextra -pthread -I. -Igames

all: main player1 player2 record

main: main.c
	$(CC) $(CFLAGS) -o main main.c -lncurses

player1: games/player1.c games/record.c
	$(CC) $(CFLAGS) -o player1 games/player1.c games/record.c -lncurses

player2: games/player2.c games/record.c
	$(CC) $(CFLAGS) -o player2 games/player2.c games/record.c -lncurses

record: games/record.c
	$(CC) $(CFLAGS) -DBUILD_RECORD_MAIN -o record games/record.c -lncurses

clean:
	rm -f main player1 player2 record *.o


