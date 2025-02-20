all: build run

build:
	mkdir -p ./bin
	gcc ./src/main.c -o ./bin/main -lSDL2 -lSDL2_ttf -I/usr/include/SDL2

run: build
	./bin/main
