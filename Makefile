main.exe: main.cpp rules.cpp
	g++ main.cpp rules.cpp -o main.exe -std=c++11
test_suite.exe: tests/main.cpp rules.cpp test.cpp
	g++ -g tests/main.cpp rules.cpp test.cpp -o test_suite.exe -std=c++11
test: test_suite.exe
	./test_suite.exe

