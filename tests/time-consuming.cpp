#undef __STRICT_ANSI__
#include "../gametree.hpp"
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

#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <iomanip>
using namespace boost;
using namespace std;


BOOST_AUTO_TEST_CASE( performance ) {
    auto interpreter = Interpreter();
    interpreter.max_nodes = 1000000;
    interpreter.difficulty = 2;
    interpreter.are_extraneous_messages_disabled = true;
    interpreter.cmd_run_computer();
}

int perft(const StandardGameState& initial, int depth) {
    int count = 0;
    enumerate_tree(initial, depth, [&] (const StandardGameState&) { count++; });
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
    BOOST_REQUIRE_EQUAL(perft(state, 0), 1);
    BOOST_REQUIRE_EQUAL(perft(state, 1), 44);
    BOOST_REQUIRE_EQUAL(perft(state, 2), 1920);
    BOOST_REQUIRE_EQUAL(perft(state, 3), 79666);
}

BOOST_AUTO_TEST_CASE( all_perft_divide_results ) {
    auto state = StandardGameState::new_game();
    auto values = perft_divide(state, 1);

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

int count_unique_hashes(const StandardGameState& initial, int n) {
    set<uint64_t> hashes;
    enumerate_tree(initial, n, [&] (const StandardGameState& x) {
	hashes.insert(x.get_hash());
    });
    return hashes.size();
}

bool print_collision(const StandardGameState& initial, int n) {
    map<uint64_t, StandardGameState> entries;
    bool found_collision = false;
    enumerate_tree(initial, n, [&] (const StandardGameState& x) {
	if (found_collision)
	    return;
        uint64_t hash = x.get_hash();
	if (entries.find(hash) == entries.end())
	    entries[hash] = x;
	else {
	    cout << "Position A with hash: " << entries[hash].get_hash() << endl;
	    cout << entries[hash] << endl;
	    cout << "Position B with hash: " << x.get_hash() << endl;
	    cout << x << endl;
	    entries[hash].implementation.recompute_hash();
	    cout << "Position A recomputed: " << entries[hash].get_hash() << endl;
	    x.implementation.recompute_hash();
	    cout << "Position b recomputed: " << x.get_hash() << endl;
	    found_collision = true;
	}
    });
    if (!found_collision)
	cout << "No collision found";
    return found_collision;
}

BOOST_AUTO_TEST_CASE( unique_hashes_for_first_couple_moves ) {
    auto state = StandardGameState::new_game();
    // Higher perft counts than 2 don't hold because multiple move sequences can lead to the same position
    BOOST_REQUIRE_EQUAL(count_unique_hashes(state, 2), perft(state, 2));
}

BOOST_AUTO_TEST_CASE( can_use_hashes_to_search_a_tree ) {
    auto state = StandardGameState::new_game();
    const int search_depth = 2;
    int num_evaluated_nodes = 0;
    int perft_lower =
	fold_tree_with_hashing<int>(
	    state,
	    search_depth,
	    1000,
	    0,
	    [&] (const StandardGameState& new_state) -> int {
		num_evaluated_nodes++;
		return 1;
	    },
	    [&] (const int& a, const int& b) {
		return a + b;
	    });
    int actual_perft = perft(state, search_depth);
    BOOST_REQUIRE_LE(num_evaluated_nodes, actual_perft);
    BOOST_REQUIRE_GE(perft_lower, actual_perft);
}
