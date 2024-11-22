all: clean compile link

link:
	g++ -o bin/Action.o bin/Auxiliary.o bin/Facility.o bin/main.o bin/Plan.o bin/SelectionPolicy.o bin/Settlement.o bin/Simulation.o

compile:
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/Action.o src/Action.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/Auxiliary.o src/Auxiliary.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/Facility.o src/Facility.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/main.o src/main.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/Plan.o src/Plan.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/SelectionPolicy.o src/SelectionPolicy.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/Settlement.o src/Settlement.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -I./include -c -o bin/Simulation.o src/Simulation.cpp

clean:
	rm -f bin/*.o


