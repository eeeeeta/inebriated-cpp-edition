all:
	clang++ -std=c++11 markov.cpp -o markov
debug:
	clang++ -std=c++11 -g markov.cpp -o markov

