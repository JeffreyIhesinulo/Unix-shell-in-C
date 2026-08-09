CC = gcc
CFLAGS = -Wall -Wextra

mush: shell.c
	$(CC) $(CFLAGS) -o mush shell.c

clean:
	rm -f mush
