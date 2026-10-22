CC      = gcc
CFLAGS  = -Wall -DHAHA
LDFLAGS = -lSDL3 -lSDL3_image -lSDL3_ttf -lpthread -lm

SRC = main.c ui.c filehandling.c algorithms.c parsing.c backend.c ssiv.c 

TARGET = ssiv

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tar: 
	tar -cf ssiv.tar *.c *.h libs Makefile
clean:
	rm -f $(TARGET)
font:
	cp -r fonts/ibmplexmono ${HOME}/.local/share/fonts/
install: all
	mv ssiv /usr/bin/ssiv
uninstall:
	rm -r ~/.cache/ssiv
	rm -r ~/.local/share/fonts/ibmplexmono
	rm /usr/bin/ssiv

.PHONY: all clean tar font install uninstall
