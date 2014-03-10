primary_files = uint128_t.o parsing.o direction.o position.o move.o piece.o scoring.o interpreter.o rules-engines/reference-gamestate.o rules-engines/reference.o  rules-engines/bitboard-gamestate.o rules-engines/bitboard-rules.o rules-engines/bitboard.o gametree.o hash.o gametree.o

home_machine_boost_include_dir = C:/mingw64/include/
options = -O3 -g -Wall -Wextra -std=c++11 -Isystem$(home_machine_boost_include_dir)
profiling_options = -p -pg
compile = g++ $(options) -o $@ -c $<
link = g++ $(options)  $^ -o $@

all: main.exe

%.o: %.cpp
	g++ $(options) -o $@ -c $<

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

main.exe: $(primary_files) main.o
	$(link) -lboost_program_options
test_chess-rules.exe: $(primary_files) tests/chess-rules.o
	$(link) -lboost_unit_test_framework
test_bitboards.exe: $(primary_files) tests/bitboards.o
	$(link) -lboost_unit_test_framework
test_foundational.exe: $(primary_files) tests/foundational.o
	$(link) -lboost_unit_test_framework
test_time-consuming.exe: $(primary_files) tests/time-consuming.o
	$(link) -lboost_unit_test_framework
test_performance.exe: $(primary_files) tests/performance.o
	$(link)

clean:
	rm $(primary_files) main.o tests/foundational.o tests/time-consuming.o tests/performance.o tests/chess-rules.o tests/bitboards.o main.exe test_chess-rules.exe test_bitboards.exe test_foundational.exe test_performance.exe test_time-consuming.exe
