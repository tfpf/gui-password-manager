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
GFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GLIBS  = $(shell pkg-config --libs gtk+-3.0)
SEARCH = -I./include/miscellaneous -I./include/classes


Source      = main.c
LExecutable = main
WExecutable = main.exe


.PHONY: clean comp exec run


clean:
	$(RM) $(LExecutable) $(WExecutable)

lcomp:
	@$(PRINT) "Compiling ...\n"
	$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(SEGVTR) $(GFLAGS) $(SEARCH) -o $(LExecutable) $(Source) $(GLIBS) $(SSL)

wcomp:
	@$(PRINT) "Compiling ...\n"
	$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(GFLAGS) $(SEARCH) -o $(WExecutable) $(Source) $(GLIBS) $(SSL)

lexec:
	@$(PRINT) "Running ...\n"
	./$(LExecutable)

wexec:
	@$(PRINT) "Running ...\n"
	./$(WExecutable)

lrun: lcomp lexec

wrun: wcomp wexec

