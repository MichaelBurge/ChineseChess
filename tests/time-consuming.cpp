#undef __STRICT_ANSI__
#include "../utility.hpp"
#include "../direction.hpp"
#include "../position.hpp"
#include "../scoring.hpp"
#include "../minimax.hpp"
#include "../interpreter.hpp"
#include "../configuration.hpp"
#include "../uint128_t.hpp"
#include <iostream>
#include <map>
#define BOOST_TEST_MAIN 1

// 'put_env' is not exported with strict ansi set
#include <cstdlib>

#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <iomanip>
using namespace boost;
using namespace std;


BOOST_AUTO_TEST_CASE( performance ) {
    auto interpreter = Interpreter();
    interpreter.max_nodes = 1000000;
    interpreter.difficulty = 2;
    interpreter.cmd_run_computer();
}

int perft(const StandardGameState& initial, int n) {
    int count = 0;
    for (const Move& move : StandardRulesEngine::available_moves(initial)) {
	if (n == 0) {
	    count += 1;
	} else {
	    initial.peek_move(move, [&] (const StandardGameState& new_state) {
	        count += perft(new_state, n - 1);
	    });
	}
    }
    return count;
}

map<Move, int> perft_divide(const StandardGameState& initial, int n) {
    auto ret = map<Move, int>();
    for (const Move& move : StandardRulesEngine::available_moves(initial)) {
	initial.peek_move(move, [&] (const StandardGameState& new_state) {
	    ret[move] = perft(new_state, n);
	});
    }
    return ret;
}

BOOST_AUTO_TEST_CASE( several_perft_cases ) {
    auto state = StandardGameState::new_game();
    BOOST_REQUIRE_EQUAL(perft(state, 0), 44);
    BOOST_REQUIRE_EQUAL(perft(state, 1), 1920);
    BOOST_REQUIRE_EQUAL(perft(state, 2), 79666);
}

BOOST_AUTO_TEST_CASE( all_perft_divide_results ) {
    auto state = StandardGameState::new_game();
    auto values = perft_divide(state, 0);

    BOOST_CHECK_EQUAL(values[Move(Position(1,1), Position(3,1))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,1), Position(2,1))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,2), Position(3,3))], 43);
    BOOST_CHECK_EQUAL(values[Move(Position(1,2), Position(3,1))], 43);
    BOOST_CHECK_EQUAL(values[Move(Position(1,3), Position(3,5))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,3), Position(3,1))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,4), Position(2,5))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,5), Position(2,5))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,6), Position(2,5))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,7), Position(3,9))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,7), Position(3,5))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,8), Position(3,9))], 43);
    BOOST_CHECK_EQUAL(values[Move(Position(1,8), Position(3,7))], 43);
    BOOST_CHECK_EQUAL(values[Move(Position(1,9), Position(2,9))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(1,9), Position(3,9))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(10,2))], 41);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(7,2))], 40);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(6,2))], 41);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(5,2))], 42);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(4,2))], 43);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(3,7))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(3,6))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(3,5))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(3,4))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(3,3))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(3,1))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,2), Position(2,2))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(10,8))], 41);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(7,8))], 40);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(6,8))], 41);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(5,8))], 42);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(4,8))], 43);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(3,9))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(3,7))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(3,6))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(3,5))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(3,4))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(3,3))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(3,8), Position(2,8))], 45);
    BOOST_CHECK_EQUAL(values[Move(Position(4,1), Position(5,1))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(4,3), Position(5,3))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(4,5), Position(5,5))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(4,7), Position(5,7))], 44);
    BOOST_CHECK_EQUAL(values[Move(Position(4,9), Position(5,9))], 44);
}
