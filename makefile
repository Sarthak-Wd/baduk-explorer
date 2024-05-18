build:
	gcc -Wall -std=c99 src/*.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -o game -g -Isrc/include
	
run:
	./game
	
clean:
	rm ./src/game
