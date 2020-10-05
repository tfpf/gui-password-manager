SHELL  = /bin/sh
RM     = rm -f
PRINT  = printf
CLEAR  = clear
CC     = gcc
CFLAGS = -O2 -s
WFLAGS = -Wall -Wextra
WIGN   = -Wno-unused-parameter
SEGVTR = -g -rdynamic
SSL    = -lcrypto -lssl
GTK    = $(shell pkg-config --libs --cflags gtk+-3.0)
SEARCH = #-I./include/miscellaneous -I./include/classes

Source     = main.c
Assembly   = main.s
Binary     = main.elf


.PHONY: clean asm comp exec run all


clean:
	$(RM) $(Binary) $(Assembly)

asm:
	@$(PRINT) "Assembling ...\n"
	$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(SEGVTR) $(SSL) $(GTK) $(SEARCH) -S -o $(Assembly) $(Source)

comp:
	@$(PRINT) "Compiling ...\n"
	$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(SEGVTR) $(SSL) $(GTK) $(SEARCH) -o $(Binary) $(Source)

exec:
	@$(PRINT) "Running ...\n"
	./$(Binary)

run: comp exec

all: asm comp exec

