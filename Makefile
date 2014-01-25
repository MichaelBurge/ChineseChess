main.exe: main.cpp rules.cpp
	g++ main.cpp rules.cpp -o main.exe -std=c++11
