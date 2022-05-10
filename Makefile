SHELL   = /bin/sh
RM      = rm -f
PRINT   = printf
CLEAR   = clear
CC      = gcc
CFLAGS  = -O2 -s -Wall -Wextra -Wno-unused-parameter -Wno-unused-result $(shell pkg-config --cflags gtk+-3.0) $(shell pkg-config --cflags libsodium)
LDFLAGS = -lcrypto -lssl $(shell pkg-config --libs gtk+-3.0) $(shell pkg-config --libs libsodium) -Wl,-rpath='$$ORIGIN'
SEGVTR  = -g -rdynamic
SEARCH  = -I./include/miscellaneous/ -I./include/classes/
WINGUI  = -mwindows


Resource    = resource.o
Source      = main.c
LExecutable = main
WExecutable = main.exe


.PHONY: clean lcomp wcomp lexec wexec lrun wrun


clean:
	$(RM) $(LExecutable) $(WExecutable)

lcomp:
	@$(PRINT) "Compiling for Linux ...\n"
	$(CC) $(CFLAGS) $(SEGVTR) $(SEARCH) -o $(LExecutable) $(Source) $(LDFLAGS)

wcomp:
	@$(PRINT) "Compiling for Windows ...\n"
	$(CC) $(CFLAGS) $(SEARCH) -o $(WExecutable) $(Source) $(Resource) $(LDFLAGS) $(WINGUI)

lexec:
	@$(PRINT) "Running ...\n"
	./$(LExecutable)

wexec:
	@$(PRINT) "Running ...\n"
	./$(WExecutable)

lrun: lcomp lexec

wrun: wcomp wexec

