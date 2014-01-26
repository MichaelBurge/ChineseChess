primary_files = rules.cpp direction.cpp position.cpp

main.exe: main.cpp $(primary_files)
	g++ -Wall -std=c++11 $^ -o $@
test_suite.exe: tests/main.cpp test.cpp $(primary_files)
	g++ -Wall -std=c++11 -g $^ -o $@
test: test_suite.exe
	./test_suite.exe

