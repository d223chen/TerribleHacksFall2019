all: main.o

main.o: main.cpp
		g++ -lncurses main.cpp -o main.o -lpthread
