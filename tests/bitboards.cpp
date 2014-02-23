#undef __STRICT_ANSI__
#include "../direction.hpp"
#include "../position.hpp"
#include "../rules-engines/reference.hpp"
#include "../scoring.hpp"
#include "../interpreter.hpp"
#include "../configuration.hpp"
#include "../uint128_t.hpp"
#include "../rules-engines/bitboard.hpp"
#include <iostream>
#define BOOST_TEST_MAIN 1

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
    BOOST_REQUIRE_EQUAL(num_set(empty.implementation.generals), (uint8_t)0);
    empty.insert_piece(Position(5,5), RED_GENERAL);
    BOOST_REQUIRE_EQUAL(num_set(empty.implementation.generals), (uint8_t)1);
    BOOST_REQUIRE_EQUAL(empty.get_piece(Position(5,5)), RED_GENERAL);
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
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(4, 5)), RED_SOLDIER);

    bitboard_state.remove_piece(Position(4, 5));
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(4, 5)), EMPTY);

    bitboard_state.insert_piece(Position(4, 5), RED_SOLDIER);
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(4, 5)), RED_SOLDIER);

    bitboard_state.apply_move(Move(Position(4, 5), Position(5, 5)));
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(4, 5)), EMPTY);
    BOOST_REQUIRE_EQUAL(bitboard_state.get_piece(Position(5, 5)), RED_SOLDIER);
}

BOOST_AUTO_TEST_CASE( precomputed_entire_board_bitboard) {
    auto entire_board = get_entire_board();
    BOOST_REQUIRE_EQUAL(num_set(entire_board), 90);
}

BOOST_AUTO_TEST_CASE( precomputed_castle_area_bitboard) {
    auto castle_area = get_castle_area();
    BOOST_REQUIRE_EQUAL(num_set(castle_area), 18);
}

BOOST_AUTO_TEST_CASE( precomputed_general_lookup_table ) {
    LookupTable table = _general_moves_lookup_table();
    bitboard castle_area = get_castle_area();
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

BOOST_AUTO_TEST_CASE( converting_bitboards_to_moves ) {
    bitboard some_moves = bitboard(0xF, 0);
    vector<Move> moves;
    insert_vectorized_moves(some_moves, Position(5, 5), moves);
    BOOST_REQUIRE_EQUAL(num_set(some_moves), 4);
    BOOST_REQUIRE_EQUAL(moves.size(), 4);
}

BOOST_AUTO_TEST_CASE( general_moves ) {
    LookupTable table = _general_moves_lookup_table();
    Position position = Position(2, 5);
    bitboard moves_board = table.boards[position.value];
    vector<Move> moves_array = vector<Move>();
    insert_vectorized_moves(moves_board, position, moves_array);
    BOOST_REQUIRE_EQUAL(num_set(moves_board), 4);
    BOOST_REQUIRE_EQUAL(moves_array.size(), 4);

    auto state = BitboardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    auto moves = _available_moves(state);
    BOOST_REQUIRE_EQUAL(moves.size(), 4);
}

BOOST_AUTO_TEST_CASE( apply_move_test) {
    auto state = BitboardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    state.insert_piece(Position(8, 6), BLACK_GENERAL);

    BOOST_REQUIRE_EQUAL(state.get_piece(Position(3, 5)), EMPTY);
    BOOST_REQUIRE_EQUAL(state.get_piece(Position(2, 5)), RED_GENERAL);

    state.apply_move(Move(Position(2, 5), Position(3, 5)));

    BOOST_REQUIRE_EQUAL(state.get_piece(Position(3, 5)), RED_GENERAL);
    BOOST_REQUIRE_EQUAL(state.get_piece(Position(2, 5)), EMPTY);
}

BOOST_AUTO_TEST_CASE( peek_move_stress_test ) {
    auto state = BitboardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    state.insert_piece(Position(8, 6), BLACK_GENERAL);

    BOOST_REQUIRE_EQUAL(state.get_piece(Position(3, 5)), EMPTY);
    BOOST_REQUIRE_EQUAL(state.get_piece(Position(2, 5)), RED_GENERAL);
    state.peek_move(Move(Position(2, 5), Position(3, 5)), [&] (const BitboardGameState& newState) {
	BOOST_REQUIRE_EQUAL(newState.get_piece(Position(3, 5)), RED_GENERAL);
	BOOST_REQUIRE_EQUAL(newState.get_piece(Position(2, 5)), EMPTY);
    });
    BOOST_REQUIRE_EQUAL(state.get_piece(Position(3, 5)), EMPTY);
    BOOST_REQUIRE_EQUAL(state.get_piece(Position(2, 5)), RED_GENERAL);
}

BOOST_AUTO_TEST_CASE( flying_kings_rule_test ) {
    auto state = BitboardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    state.insert_piece(Position(8, 5), BLACK_GENERAL);
    BOOST_REQUIRE(violates_flying_kings_rule(state));
}

BOOST_AUTO_TEST_CASE( horse_moves_test) {
    auto state = GameState<BitboardGameState>::new_game();
    auto horse_moves = moves_for_piece(state.implementation, Position(1, 2), RED_HORSE);
    BOOST_REQUIRE_EQUAL(num_set(horse_moves), 2);

    state.apply_move(Move(Position(1,1), Position(3, 1)));
    horse_moves = moves_for_piece(state.implementation, Position(10, 2), BLACK_HORSE);
    BOOST_REQUIRE_EQUAL(num_set(horse_moves), 2);
}
