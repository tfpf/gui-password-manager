SHELL  = /bin/sh
RM     = rm -f
PRINT  = printf
CC     = gcc
WC     = x86_64-w64-mingw32-gcc
CFLAGS = -O0
WFLAGS = -Wall -Wextra
WIGN   = -Wno-unused-parameter
LFLAGS = -lcrypto -lssl
INCLD  = $(shell pkg-config --libs --cflags gtk+-3.0)
SEARCH = -Iinclude/miscellaneous -Iinclude/classes
# install GTK3 with the following command
# sudo apt install libgtk-3-dev
# https://askubuntu.com/questions/942010/how-to-compile-c-gtk3-program-in-ubuntu-for-windows

Source     = main.c
Assembly   = main.s
Binary     = main.elf
Executable = main.exe


.PHONY: clean asm comp pe exec run all


clean:
	$(RM) $(Binary) $(Assembly)

asm:
	@clear
	$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(LFLAGS) $(INCLD) $(SEARCH) -S -o $(Assembly) $(Source)

comp:
	@$(PRINT) "Compiling ...\n"
	@$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(LFLAGS) $(INCLD) $(SEARCH) -o $(Binary) $(Source)

pe:
	@$(PRINT) "Compiling ...\n"
	@$(WC) $(CFLAGS) $(WFLAGS) $(WIGN) $(LFLAGS) $(INCLD) -o $(Executable) $(Source)

exec:
	@$(PRINT) "Running ...\n"
	@./$(Binary)

run: comp exec

all: asm comp exec

