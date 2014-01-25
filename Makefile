main.exe: main.cpp rules.cpp
	g++ main.cpp rules.cpp -o main.exe -std=c++11
test:
	g++ -g test.cpp rules.cpp -o test.exe -std=c++11

