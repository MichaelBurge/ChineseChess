primary_files = uint128_t.o parsing.o direction.o position.o move.o player.o piece.o minimax.o scoring.o interpreter.o rules-engines/reference-gamestate.o rules-engines/reference.o  rules-engines/bitboard-gamestate.o rules-engines/bitboard-rules.o rules-engines/bitboard.o

test_files = test.o

options = -g -Wall -std=c++11

%.o: %.cpp
	g++ $(options) -o $@ -c $<

all: main.exe

main.exe: $(primary_files) main.o
	g++ $(options) $^ -o $@

performance: test_performance.exe
	./test_performance.exe

test: test_chess-rules.exe test_bitboards.exe test_foundational.exe test_time-consuming.exe
	./test_foundational.exe
	./test_bitboards.exe
	./test_chess-rules.exe
	./test_time-consuming.exe

test_chess-rules.exe: $(primary_files) $(test_files) tests/chess-rules.o
	g++ $(options) $^ -o $@
test_bitboards.exe: $(primary_files) $(test_files) tests/bitboards.o
	g++ $(options) $^ -o $@
test_foundational.exe: $(primary_files) $(test_files) tests/foundational.o
	g++ $(options) $^ -o $@
test_time-consuming.exe: $(primary_files) $(test_files) tests/time-consuming.o
	g++ $(options) $^ -o $@
test_performance.exe: $(primary_files) $(test_files) tests/performance.o
	g++ -p -pg $(options) $^ -o $@

clean:
	rm $(primary_files) $(test_files) main.o tests/performance.o main.exe test_chess-rules.exe test_bitboards.exe test_foundational.exe test_performance.exe test_time-consuming.exe
