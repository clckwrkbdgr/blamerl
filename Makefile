all:
	g++ -o blamerl main.cpp -lncurses
	xfce4-terminal -e './blamerl'
