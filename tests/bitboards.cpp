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
using namespace bitboard_implementation;

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

BOOST_AUTO_TEST_CASE( precomputed_entire_board_bitboard) {
    auto entire_board = generate_entire_board();
    BOOST_REQUIRE_EQUAL(num_set(entire_board), 90);
}

BOOST_AUTO_TEST_CASE( precomputed_castle_area_bitboard) {
    auto castle_area = generate_castle_area();
    BOOST_REQUIRE_EQUAL(num_set(castle_area), 18);
}

BOOST_AUTO_TEST_CASE( precomputed_general_lookup_table ) {
    LookupTable table = generate_general_moves_lookup_table();
    bitboard castle_area = generate_castle_area();
    int num_positions_checked = 0;
    for (uint8_t position = 0; position < 90; position++) {
	if (!castle_area.get(position))
	    continue;
	BOOST_TEST_CHECKPOINT( "Checking position: " << Position(position));
	BOOST_REQUIRE_GE(num_set(table.boards[position]), 2);
	BOOST_REQUIRE_LE(num_set(table.boards[position]), 4);
	num_positions_checked++;
    }
    BOOST_REQUIRE_EQUAL(num_positions_checked, 18);
}
