
build: cpp_cache_experiment cpp_cache_experiment_avx


OPT=-Wall -std=c++0x -O1

cpp_cache_experiment_avx: main_avx.o
	g++ -o $@ $(OPT) $^

cpp_cache_experiment: main.o
	g++ -o $@ $(OPT) $^

main_avx.o: main.cpp
	g++ -mavx -S -c $(OPT) $<
	g++ -mavx -c $(OPT) $< -o $@
	
main.o: main.cpp
	g++ -S -c $(OPT) $<
	g++ -c $(OPT) $< -o $@
	

