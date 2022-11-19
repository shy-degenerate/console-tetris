LIBS= -lncurses -pthread

default:
	g++ tetris.cpp $(LIBS) -o tetris
