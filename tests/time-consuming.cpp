#undef __STRICT_ANSI__
#include "../utility.hpp"
#include "../direction.hpp"
#include "../position.hpp"
#include "../test.hpp"
#include "../scoring.hpp"
#include "../minimax.hpp"
#include "../interpreter.hpp"
#include "../configuration.hpp"
#include "../uint128_t.hpp"
#include <iostream>
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

BOOST_AUTO_TEST_CASE( several_perft_cases ) {
    auto state = StandardGameState::new_game();
    assert_eq(perft(state, 0), 44, "Failed perft(0)");
    assert_eq(perft(state, 1), 1920, "Failed perft(1)");
    assert_eq(perft(state, 2), 79666, "Failed perft(2)");
}
