CC = gcc
CFLAGS ?= -Wall -Wextra -Werror -Iinclude

all: main.c
	$(CC) $(CFLAGS) -o main $^

debug: main.c
	$(CC) -DDEBUG $(CFLAGS) -o main $^