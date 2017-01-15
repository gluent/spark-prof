
build: cpp_cache_experiment


OPT=-Wall -std=c++0x -O1 -g

cpp_cache_experiment: main.o
	g++ -o $@ $(OPT) $^

main.o: main.cpp
	g++ -S -c $(OPT) $<
	g++ -c $(OPT) $< -o $@
	

