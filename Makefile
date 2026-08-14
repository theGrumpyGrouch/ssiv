CC      = gcc
CFLAGS  = -Wall
LDFLAGS = -lSDL3 -lSDL3_image -lSDL3_ttf -lpthread -lm

SRC = main.c ui.c filehandling.c algorithms.c ssiv.c 

TARGET = ssiv

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
