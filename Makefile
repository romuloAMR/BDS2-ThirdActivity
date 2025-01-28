CXX = g++
FLAGS = -Wall -pedantic -Ofast -march=native

all: main
	./main
	dot -Tpng arvoreB.dot -o img/arvoreB.png
	dot -Tpng arvoreBREM.dot -o img/arvoreBREM.png
	del /Q *.dot
	del /Q main.exe
	del /Q Objects\*.o

main: Objects/main.o
	$(CXX) $(FLAGS) $^ -o main

Objects/main.o: mainTest.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	del /Q main.exe
	del /Q Objects\*.o
