primary_files = rules.cpp parsing.cpp direction.cpp position.cpp move.cpp minimax.cpp scoring.cpp

main.exe: main.cpp $(primary_files) interpreter.cpp
	g++ -Wall -std=c++11 $^ -o $@
test_suite.exe: tests/main.cpp test.cpp $(primary_files)
	g++ -Wall -std=c++11 -g $^ -o $@
test: test_suite.exe
	./test_suite.exe

