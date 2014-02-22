#undef __STRICT_ANSI__
#include "../direction.hpp"
#include "../position.hpp"
#include "../rules-engines/reference.hpp"
#include "../scoring.hpp"
#include "../minimax.hpp"
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

Position center_of_castle() { return Position(2, 5); }
Position center_of_board() { return Position(5, 5); }

BOOST_AUTO_TEST_CASE( general) {
  auto state = StandardGameState(RED);
  state.insert_piece(center_of_castle(), RED_GENERAL);
  BOOST_CHECK_EQUAL(StandardRulesEngine::num_available_moves(state), 4);
}

BOOST_AUTO_TEST_CASE( advisor ) {
  auto state = StandardGameState(RED);
  state.insert_piece(center_of_castle(), RED_ADVISOR);

  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 4);

  with_90_degree_rotations(SOUTHEAST, [] (Direction direction) {
      auto state = StandardGameState(RED);
      state.insert_piece(move_direction(center_of_castle(), direction), RED_ADVISOR);
      BOOST_CHECK_EQUAL(StandardRulesEngine::num_available_moves(state), 1);
  });
}

BOOST_AUTO_TEST_CASE( horse ) {
  auto horsePosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(horsePosition, RED_HORSE);

  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 8);

  state.insert_piece(
      move_direction(horsePosition, NORTH),
      BLACK_SOLDIER);
  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 6);
}

BOOST_AUTO_TEST_CASE( elephant ) {
    auto elephantPosition = Position(3, 5);
    auto state = StandardGameState(RED);
    state.insert_piece(elephantPosition, RED_ELEPHANT);

    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 4);
    state.insert_piece(
        move_direction(elephantPosition, NORTHEAST),
	BLACK_SOLDIER);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 3);
}

BOOST_AUTO_TEST_CASE( elephant_cant_cross_river ) {
    auto state = StandardGameState(RED);
    state.insert_piece(Position(5, 3), RED_ELEPHANT);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 2);
}

BOOST_AUTO_TEST_CASE( chariot ) {
  auto chariotPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(chariotPosition, RED_CHARIOT);

  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 5+4+4+4);

  state.insert_piece(
      move_direction(chariotPosition, NORTH),
      BLACK_SOLDIER);
  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 1+4+4+4);

  state.insert_piece(
      move_direction(chariotPosition, WEST),
      BLACK_SOLDIER);
  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 1+1+4+4);
}

BOOST_AUTO_TEST_CASE( cannon ) {
  auto cannonPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(cannonPosition, RED_CANNON);

  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 5+4+4+4);

  state.insert_piece(
      move_direction(cannonPosition, NORTH),
      BLACK_SOLDIER);
  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 0+4+4+4);

  state.insert_piece(
      move_direction(move_direction(cannonPosition, NORTH), NORTH),
      BLACK_ELEPHANT);

  BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 1+4+4+4);

  BOOST_TEST_CHECKPOINT("Double-hopping bug");
  state = StandardGameState(RED);
  state.insert_piece(Position(10, 8), RED_CANNON);
  state.insert_piece(Position(9, 8), BLACK_ELEPHANT);
  state.insert_piece(Position(8, 8), BLACK_ADVISOR);
  state.insert_piece(Position(7, 8), BLACK_GENERAL);
  BOOST_REQUIRE(!StandardRulesEngine::is_legal_move(state, Move(Position(10, 8), Position(7, 8))));
}

BOOST_AUTO_TEST_CASE( soldier ) {
    auto position = center_of_board();

    BOOST_TEST_CHECKPOINT("Soldier pre-river");
    auto state = StandardGameState(RED);
    state.insert_piece(position, RED_SOLDIER);

    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 1);

    BOOST_TEST_CHECKPOINT("Soldier post-river");
    state = StandardGameState(RED);
    auto north = move_direction(position, NORTH);
    state.insert_piece(
	north,
	RED_SOLDIER);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 3);
}

BOOST_AUTO_TEST_CASE( piece_capture ) {
    auto position = center_of_board();
    auto piece = RED_CHARIOT;
    auto ally  = RED_CHARIOT;
    auto enemy = BLACK_CHARIOT;

    auto state = StandardGameState(RED);
    state.insert_piece(position, piece);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_captures(state), 0);

    state.insert_piece(
        move_direction(position, NORTH),
        ally);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_captures(state), 0);

    state.insert_piece(
        move_direction(position, WEST),
        enemy);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_captures(state), 1);
}

BOOST_AUTO_TEST_CASE( flying_kings_rule ) {
    auto state = StandardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 4);

    state.insert_piece(Position(8, 6), BLACK_GENERAL);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 3);

    state.insert_piece(Position(5, 6), BLACK_SOLDIER);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 4);
}



BOOST_AUTO_TEST_CASE( winning ) {
    auto state = StandardGameState::new_game();
    BOOST_REQUIRE(!StandardRulesEngine::is_winner(state));

    state = StandardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    state.insert_piece(Position(1, 8), BLACK_CANNON);
    state.insert_piece(Position(1, 7), BLACK_SOLDIER);
    state.insert_piece(Position(2, 7), BLACK_CHARIOT);
    state.insert_piece(Position(3, 7), BLACK_CHARIOT);
    state.insert_piece(Position(8, 6), BLACK_GENERAL);

    BOOST_REQUIRE(StandardRulesEngine::is_winner(state));
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::winner(state), BLACK);
}

BOOST_AUTO_TEST_CASE( game_state_dictionary_storage ) {
    auto x = ReferenceGameStateDictionaryStorage();
    x.insert_piece(Position(2, 5), RED_CHARIOT);
    x.insert_piece(Position(2, 5), RED_CHARIOT);
    BOOST_REQUIRE_EQUAL(x.size(), 1);
    x.insert_piece(Position(2, 6), BLACK_CHARIOT);
}

BOOST_AUTO_TEST_CASE( data_structures ) {
    auto game = StandardGameState::new_game();
    auto num_pieces = 0;
    game.for_each_piece([&] (Position, Piece) {
        num_pieces++;
    });
    BOOST_REQUIRE_EQUAL(num_pieces, 32);

    auto state = StandardGameState(RED);
    auto old_position = Position(2, 5);
    auto new_position = move_direction(old_position, WEST);
    state.insert_piece(old_position, RED_GENERAL);
    state.insert_piece(move_direction(old_position, NORTH), RED_CHARIOT);
    state.insert_piece(Position(9, 1), BLACK_CHARIOT);
    state.peek_move(Move(old_position, new_position), [&] (const StandardGameState newState) {
	newState.peek_move(Move(Position(9, 1), Position(9, 2)), [&] (const StandardGameState&) {
	    newState.peek_move(Move(new_position, old_position), [] (const StandardGameState&) {
	    });
	});
    });
    BOOST_REQUIRE_EQUAL(state.current_turn(), RED);
    best_move(state, 3, 1000, piece_score);
    BOOST_REQUIRE_EQUAL(state.current_turn(), RED);

    BOOST_REQUIRE_EQUAL(state.get_piece(old_position), RED_GENERAL);
    BOOST_REQUIRE_EQUAL(state.get_piece(move_direction(old_position, NORTH)), RED_CHARIOT);
    BOOST_REQUIRE_EQUAL(state.get_piece(Position(9, 1)), BLACK_CHARIOT);
}

BOOST_AUTO_TEST_CASE( check ) {
    auto state = StandardGameState(RED);
    auto position = center_of_castle();
    state.insert_piece(position, RED_GENERAL);
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 4);

    state.insert_piece(Position(6, 5), BLACK_CHARIOT);

    BOOST_REQUIRE(StandardRulesEngine::is_king_in_check(state, RED));

    BOOST_REQUIRE(StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, NORTH))));
    BOOST_REQUIRE(StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, SOUTH))));
    BOOST_REQUIRE(!StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, WEST))));
    BOOST_REQUIRE(!StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, EAST))));
    BOOST_REQUIRE_EQUAL(StandardRulesEngine::num_available_moves(state), 2);

    state.insert_piece(Position(4, 5), RED_CHARIOT);
    BOOST_REQUIRE(!StandardRulesEngine::is_king_in_check(state, RED));

    BOOST_TEST_CHECKPOINT("Testing the 'you can't take my general who's in check because you're in check' problem");
    state = StandardGameState(BLACK);
    state.insert_piece(Position(1, 5),  RED_GENERAL);
    state.insert_piece(Position(1, 6),  RED_ADVISOR);
    state.insert_piece(Position(10, 5), BLACK_GENERAL);
    state.insert_piece(Position(10, 6), BLACK_CHARIOT);
    state.insert_piece(Position(10, 7), RED_CHARIOT);
    state.insert_piece(Position(2, 5),  RED_SOLDIER);
    state.insert_piece(Position(2, 1),  BLACK_CHARIOT);
    state.insert_piece(Position(2, 7),  BLACK_HORSE);
    auto bad_move = Move(Position(10,6), Position(1,6));
    BOOST_REQUIRE(!StandardRulesEngine::is_legal_move(state, bad_move));
}

BOOST_AUTO_TEST_CASE( scoring ) {
    auto state = StandardGameState(RED);
    auto ally_king_position = Position(2, 5);
    auto chariot_position = Position(5, 5);
    auto enemy_king_position = Position(8, 5);
    state.insert_piece(enemy_king_position, BLACK_GENERAL);
    state.insert_piece(chariot_position,    RED_CHARIOT);
    state.insert_piece(ally_king_position,  RED_GENERAL);
    BOOST_REQUIRE_EQUAL(piece_score(state), piece_value(RED_CHARIOT));
    state.apply_move(Move(chariot_position, enemy_king_position));
    BOOST_REQUIRE_EQUAL(piece_score(state), -(piece_value(RED_CHARIOT) + piece_value(RED_GENERAL)));
}

BOOST_AUTO_TEST_CASE( basic_minimax ) {
    auto state = StandardGameState(RED);
    auto ally_king_position  = Position(2, 5);
    auto enemy_derp_position = Position(3, 3);
    auto chariot_position    = Position(5, 5);
    auto enemy_king_position = Position(8, 4);
      
    state.insert_piece(enemy_king_position, BLACK_GENERAL);
    state.insert_piece(enemy_derp_position, BLACK_CHARIOT);
    state.insert_piece(chariot_position,    RED_CHARIOT);
    state.insert_piece(ally_king_position,  RED_GENERAL);

    auto best = Move(chariot_position, Position(5, 4));
    auto ai_move = best_move(state, 3, 1000, piece_score);
    BOOST_REQUIRE_EQUAL(ai_move, best);
}

BOOST_AUTO_TEST_CASE( num_piece_moves_at_start_of_game ) {
    auto state = StandardGameState::new_game();
    auto check_piece = [&] (Position position, int expected, Piece piece) {
	BOOST_TEST_MESSAGE("Checking piece " << piece << " at position " << position);
	BOOST_CHECK_EQUAL(StandardRulesEngine::available_moves_from(state, position).size(), expected);	
    };
    check_piece(Position(1, 1), 2, RED_CHARIOT);
    check_piece(Position(1, 2), 2, RED_HORSE);
    check_piece(Position(1, 3), 2, RED_ELEPHANT);
    check_piece(Position(1, 4), 1, RED_ADVISOR);
    check_piece(Position(1, 5), 1, RED_GENERAL);
    check_piece(Position(1, 6), 1, RED_ADVISOR);
    check_piece(Position(1, 7), 2, RED_ELEPHANT);
    check_piece(Position(1, 8), 2, RED_HORSE);
    check_piece(Position(1, 9), 2, RED_CHARIOT);

    check_piece(Position(3, 2), 5+1+1+5, RED_CANNON);
    check_piece(Position(3, 2), 5+1+1+5, RED_CANNON);

    check_piece(Position(4, 1), 1, RED_SOLDIER);
    check_piece(Position(4, 3), 1, RED_SOLDIER);
    check_piece(Position(4, 5), 1, RED_SOLDIER);
    check_piece(Position(4, 7), 1, RED_SOLDIER);
    check_piece(Position(4, 9), 1, RED_SOLDIER);
}
