CC = gcc
CFLAGS = -Wall -g -I/mingw64/include/SDL2 -O3 -s -DNDBUG -std=c99 -mwindows -pedantic
LDFLAGS = -L/mingw64/lib -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

all: asteroids

asteroids: main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f asteroids