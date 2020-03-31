SHELL  = /bin/sh
RM     = rm
PRINT  = printf
CC     = gcc
CFLAGS = -O0
WFLAGS = -Wall -Wextra
WIGN   = -Wno-unused-parameter
LFLAGS = -lcrypto -lssl
INCLD  = -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
# INCLD is the output of the following command
# pkg-config --libs --cflags gtk+-3.0
# it required installing GTK3 using the following
# sudo apt install libgtk-3-dev

Source   = main.c
Assembly = main.s
Binary   = main.elf


.PHONY: clean asm comp win32 exec run all


clean:
	$(RM) -f $(Binary) $(Assembly)

asm:
	$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(LFLAGS) $(INCLD) -s -o $(Assembly) $(Source)

comp:
	@$(PRINT) "Compiling ...\n"
	@$(CC) $(CFLAGS) $(WFLAGS) $(WIGN) $(LFLAGS) $(INCLD) -o $(Binary) $(Source)

win32:
	@$(PRINT) "Placeholder for after I have installed MINGW.\n"

exec:
	@$(PRINT) "Running ...\n"
	@./$(Binary)

run: comp exec

all: asm comp exec

