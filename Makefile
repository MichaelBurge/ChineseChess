primary_files = uint128_t.o parsing.o direction.o position.o move.o piece.o minimax.o scoring.o interpreter.o rules-engines/reference-gamestate.o rules-engines/reference.o  rules-engines/bitboard-gamestate.o rules-engines/bitboard-rules.o rules-engines/bitboard.o

options = -O3 -g -Wall -std=c++11
profiling_options = -p -pg
compile = g++ $(options) -o $@ -c $<
link = g++ $(options) $^ -o $@

%.o: %.cpp
	g++ $(options) -o $@ -c $<

all: main.exe

main.exe: $(primary_files) main.o
	$(compile)

rules-engines/bitboard-gamestate.o: rules-engines/bitboard-gamestate.cpp
	$(compile) -fno-exceptions
rules-engines/bitboard-rules.o: rules-engines/bitboard-rules.cpp
	$(compile) -fno-exceptions

performance: test_performance.exe
	./test_performance.exe

test: test_chess-rules.exe test_bitboards.exe test_foundational.exe test_time-consuming.exe
	./test_foundational.exe
	./test_bitboards.exe
	./test_chess-rules.exe
	./test_time-consuming.exe

test_chess-rules.exe: $(primary_files) tests/chess-rules.o
	$(link)
test_bitboards.exe: $(primary_files) tests/bitboards.o
	$(link)
test_foundational.exe: $(primary_files) tests/foundational.o
	$(link)
test_time-consuming.exe: $(primary_files) tests/time-consuming.o
	$(link)
test_performance.exe: $(primary_files) tests/performance.o
	$(link)

clean:
	rm $(primary_files) main.o tests/foundational.o tests/time-consuming.o tests/performance.o tests/chess-rules.o tests/bitboards.o main.exe test_chess-rules.exe test_bitboards.exe test_foundational.exe test_performance.exe test_time-consuming.exe
