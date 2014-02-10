primary_files = rules.o parsing.o direction.o position.o move.o minimax.o scoring.o interpreter.o
options = -Wall -std=c++11 -g -pg

%.o: %.cpp
	g++ $(options) -o $@ -c $<

main.exe: main.o $(primary_files)
	g++ $(options) $^ -o $@
test_suite.exe: tests/main.o test.o $(primary_files)
	g++ $(options) $^ -o $@
test: test_suite.exe
	./test_suite.exe

