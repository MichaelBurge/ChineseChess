primary_files = rules.o parsing.o direction.o position.o move.o minimax.o scoring.o interpreter.o gamestate.o
test_files = tests/main.o test.o
performance_files = tests/performance.o

options = -O2 -Wall -std=c++11

%.o: %.cpp
	g++ $(options) -o $@ -c $<

main.exe: main.o $(primary_files)
	g++ $(options) $^ -o $@
test_suite.exe: $(test_files) $(primary_files)
	g++ $(options) $^ -o $@
test: test_suite.exe
	./test_suite.exe

performance_test.exe: $(performance_files) $(primary_files)
	g++ $(options) $^ -o $@

performance: performance_test.exe
	./performance_test.exe

clean:
	rm $(primary_files) $(test_files) $(performance_files) main.exe test_suite.exe performance_test.exe
