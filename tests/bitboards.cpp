#undef __STRICT_ANSI__
#include "../utility.hpp"
#include "../direction.hpp"
#include "../parsing.hpp"
#include "../position.hpp"
#include "../rules-engines/reference.hpp"
#include "../test.hpp"
#include "../scoring.hpp"
#include "../minimax.hpp"
#include "../interpreter.hpp"
#include "../configuration.hpp"
#include "../uint128_t.hpp"
#include "../rules-engines/bitboard.hpp"
#include <iostream>
#define BOOST_TEST_MAIN 1
#define BOOST_TEST_MODULE "chess tests"

// 'put_env' is not exported with strict ansi set
#include <cstdlib>

#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <iomanip>
using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE( bitboard_get_piece ) {
    typedef GameState<BitboardGameState> bitboard_s;
    auto empty = bitboard_s(RED);
    assert_eq(num_set(empty.implementation.generals), (uint8_t)0, "Board not empty at start");
    empty.insert_piece(Position(5,5), RED_GENERAL);
    assert_eq(num_set(empty.implementation.generals), (uint8_t)1, "Wrong number of generals after insert");
    assert_eq(empty.get_piece(Position(5,5)), RED_GENERAL, "Wrong piece at location");
}

BOOST_AUTO_TEST_CASE( removing_and_adding_pieces ) {
    typedef GameState<BitboardGameState> bitboard_s;

    auto bitboard_state = bitboard_s::new_game();
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(4, 5)), RED_SOLDIER);
    BOOST_CHECK_EQUAL(get_num_pieces(bitboard_state), 32);

    bitboard_state.remove_piece(Position(4, 5));
    BOOST_CHECK_EQUAL(bitboard_state.get_piece(Position(4, 5)), EMPTY);
    BOOST_CHECK_EQUAL(get_num_pieces(bitboard_state), 31);

    bitboard_state.insert_piece(Position(4, 5), RED_SOLDIER);
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(4, 5)), RED_SOLDIER);
    BOOST_CHECK_EQUAL(get_num_pieces(bitboard_state), 32);
}

BOOST_AUTO_TEST_CASE( operations_on_bitboards ) {
    typedef GameState<BitboardGameState> bitboard_s;
    auto bitboard_state = bitboard_s::new_game();
    assert_eq(bitboard_state.get_piece(Position(4, 5)), RED_SOLDIER, "Not a red soldier");

    bitboard_state.remove_piece(Position(4, 5));
    bitboard_state.insert_piece(Position(4, 5), RED_SOLDIER);

    bitboard_state.apply_move(Move(Position(4, 5), Position(5, 5)));
}
