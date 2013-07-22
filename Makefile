all: main.o game.o console.o
	g++ -o blamerl -lncurses main.o game.o console.o
	xfce4-terminal -e './blamerl'

game.o: game.h
main.o: game.h console.h
console.o: console.h

clean:
	rm -rf *.o blamerl
