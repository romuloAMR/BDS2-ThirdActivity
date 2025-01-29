CXX = g++
FLAGS = -Wall -pedantic -Ofast -march=native

all: main
	./main
	dot -Tpng arvoreB.dot -o arvoreB.png
	dot -Tpng arvoreBREM.dot -o arvoreBREM.png
	rm main
	rm -f main Objects/*.o
	rm -f *.dot
	

main: Objects/main.o
	$(CXX) $(FLAGS) $^ -o main

test: Objects/mainTest.o
	$(CXX) $(FLAGS) $^ -o mainTest

Objects/main.o: main.cpp
	$(CXX) $(FLAGS) -c $< -o $@

Objects/mainTest.o: mainTest.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f main Objects/*.o
	