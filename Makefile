primary_files = uint128_t.o parsing.o direction.o position.o move.o player.o piece.o minimax.o scoring.o interpreter.o rules-engines/reference-gamestate.o rules-engines/reference.o  rules-engines/bitboard-gamestate.o rules-engines/bitboard-rules.o rules-engines/bitboard.o

test_files = tests/main.o test.o
performance_files = tests/performance.o

options = -O2 -g -Wall -std=c++11

%.o: %.cpp
	g++ $(options) -o $@ -c $<

main.exe: $(primary_files) main.o
	g++ $(options) $^ -o $@
test_suite.exe: $(test_files) $(primary_files)
	g++ $(options) $^ -o $@
test: test_suite.exe
	./test_suite.exe

performance_test.exe: $(performance_files) $(primary_files)
	g++ -p -pg $(options) $^ -o $@

performance: performance_test.exe
	./performance_test.exe

clean:
	rm $(primary_files) $(test_files) $(performance_files) main.exe test_suite.exe performance_test.exe
