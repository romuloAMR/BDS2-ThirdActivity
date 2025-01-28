CXX = g++
FLAGS = -Wall -pedantic -Ofast -march=native

all: main
	./main
	rm main
	rm -f main Objects/*.o

main: Objects/main.o
	$(CXX) $(FLAGS) $^ -o main

Objects/main.o: mainTest.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f main Objects/*.o