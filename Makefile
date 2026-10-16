CC      = gcc
CFLAGS  = -Wall
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
install: all	
	mv ssiv /usr/bin/ssiv
uninstall:
	rm /usr/bin/ssiv

.PHONY: all clean tar install
