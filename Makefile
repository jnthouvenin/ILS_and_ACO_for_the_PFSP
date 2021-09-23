
program : src/pfspinstance.h src/localSearch.h src/pfspinstance.cpp src/globalVariables.h src/files.h src/errors.h src/parameters.h src/experiment.h src/aco.h src/ils.h src/main.cpp
	g++ -O3 -c ./src/pfspinstance.cpp -o src/pfspinstance.o
	g++ -O3 -c ./src/main.cpp -o src/main.o

	g++ -O3 src/main.o src/pfspinstance.o -o main

clean:
	rm src/*.o main
