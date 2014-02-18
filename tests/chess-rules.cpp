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

  assert_eq(StandardRulesEngine::num_available_moves(state), 8, "Incorrect number of horse moves 1");

  state.insert_piece(
      move_direction(horsePosition, NORTH),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 6, "Horse's leg can't be tripped");
}

BOOST_AUTO_TEST_CASE( elephant ) {
  auto elephantPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(elephantPosition, RED_ELEPHANT);

  assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect number of elephant moves");
  state.insert_piece(
      move_direction(elephantPosition, NORTHEAST),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 3, "Can't block the elephant's eye");
}

BOOST_AUTO_TEST_CASE( chariot ) {
  auto chariotPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(chariotPosition, RED_CHARIOT);

  assert_eq(StandardRulesEngine::num_available_moves(state), 5+4+4+4, "Incorrect number of chariot moves 1");

  state.insert_piece(
      move_direction(chariotPosition, NORTH),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 1+4+4+4, "Incorrect number of chariot moves 2");

  state.insert_piece(
      move_direction(chariotPosition, WEST),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 1+1+4+4, "Incorrect number of chariot moves 3");
}

BOOST_AUTO_TEST_CASE( cannon ) {
  auto cannonPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(cannonPosition, RED_CANNON);

  assert_eq(StandardRulesEngine::num_available_moves(state), 5+4+4+4, "Incorrect number of cannon moves(no obstructions)");

  state.insert_piece(
      move_direction(cannonPosition, NORTH),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 0+4+4+4, "Incorrect number of cannon moves(one obstruction)");

  state.insert_piece(
      move_direction(move_direction(cannonPosition, NORTH), NORTH),
      BLACK_ELEPHANT);
  assert_eq(StandardRulesEngine::num_available_moves(state), 1+4+4+4, "Incorrect number of cannon moves(one capture)");

  state = StandardGameState(RED);
  state.insert_piece(Position(10, 8), RED_CANNON);
  state.insert_piece(Position(9, 8), BLACK_ELEPHANT);
  state.insert_piece(Position(8, 8), BLACK_ADVISOR);
  state.insert_piece(Position(7, 8), BLACK_GENERAL);
  deny(StandardRulesEngine::is_legal_move(state, Move(Position(10, 8), Position(7, 8))), "Double-hopping bug");
}

BOOST_AUTO_TEST_CASE( soldier ) {
    auto position = center_of_board();
    auto state = StandardGameState(RED);
    state.insert_piece(position, RED_SOLDIER);

    assert_eq(StandardRulesEngine::num_available_moves(state), 1, "Incorrect number of pre-river soldier moves");

    state = StandardGameState(RED);
    state.insert_piece(
        move_direction(position, NORTH),
	RED_SOLDIER);

    assert_eq(StandardRulesEngine::num_available_moves(state), 3, "Incorrect number of post-river soldier moves");
}

BOOST_AUTO_TEST_CASE( piece_capture ) {
    auto position = center_of_board();
    auto piece = RED_CHARIOT;
    auto ally  = RED_CHARIOT;
    auto enemy = BLACK_CHARIOT;

    auto state = StandardGameState(RED);
    state.insert_piece(position, piece);
    assert_eq(StandardRulesEngine::num_available_captures(state), 0, "Captures incorrectly found 1");

    state.insert_piece(
        move_direction(position, NORTH),
        ally);
    assert_eq(StandardRulesEngine::num_available_captures(state), 0, "Allies can be captured");

    state.insert_piece(
        move_direction(position, WEST),
        enemy);
    assert_eq(StandardRulesEngine::num_available_captures(state), 1, "Capture move is not recorded properly");
}

BOOST_AUTO_TEST_CASE( flying_kings_rule ) {
    auto state = StandardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect # of king moves");

    state.insert_piece(Position(8, 6), BLACK_GENERAL);
    assert_eq(StandardRulesEngine::num_available_moves(state), 3, "Enemy king doesn't block a move");

    state.insert_piece(Position(5, 6), BLACK_SOLDIER);
    assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Soldier doesn't block flying kings");
}



BOOST_AUTO_TEST_CASE( winning ) {
    auto state = StandardGameState::new_game();
    deny(StandardRulesEngine::is_winner(state), "Winner at start of game");

    state = StandardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    state.insert_piece(Position(2, 7), BLACK_CHARIOT);
    state.insert_piece(Position(3, 7), BLACK_CHARIOT);
    state.insert_piece(Position(8, 5), BLACK_GENERAL);

    _assert(StandardRulesEngine::is_winner(state), "No winner");
    assert_eq(StandardRulesEngine::winner(state), BLACK, "Wrong winner");
}

BOOST_AUTO_TEST_CASE( game_state_dictionary_storage ) {
    auto x = ReferenceGameStateDictionaryStorage();
    x.insert_piece(Position(2, 5), RED_CHARIOT);
    x.insert_piece(Position(2, 5), RED_CHARIOT);
    assert_eq(x.size(), 1, "Insert two elements");
    x.insert_piece(Position(2, 6), BLACK_CHARIOT);
}

BOOST_AUTO_TEST_CASE( data_structures ) {
    auto game = StandardGameState::new_game();
    auto num_pieces = 0;
    game.for_each_piece([&] (Position, Piece) {
        num_pieces++;
    });
    assert_eq(num_pieces, 32, "for_each_piece is broken");

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
    assert_eq(state.current_turn(), RED, "current_turn not preserved 1");
    best_move(state, 3, 1000, piece_score);
    assert_eq(state.current_turn(), RED, "current_turn not preserved 2");

    assert_eq(state.get_piece(old_position), RED_GENERAL, "General non-existent or in the wrong place");
    assert_eq(state.get_piece(move_direction(old_position, NORTH)), RED_CHARIOT, "Red chariot non-existent or in the wrong place");
    assert_eq(state.get_piece(Position(9, 1)), BLACK_CHARIOT, "Black chariot non-existent or in the wrong place");
}

BOOST_AUTO_TEST_CASE( check ) {
    auto state = StandardGameState(RED);
    auto position = center_of_castle();
    state.insert_piece(position, RED_GENERAL);
    assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect number of general moves");

    state.insert_piece(Position(6, 5), BLACK_CHARIOT);
    _assert(StandardRulesEngine::is_king_in_check(state, RED), "King not scared of chariot");
    _assert(StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, NORTH))), "NORTH check");
    _assert(StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, SOUTH))), "SOUTH check");
    deny(StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, WEST))), "WEST check");
    deny(StandardRulesEngine::results_in_check(state, Move(position, move_direction(position, EAST))), "EAST check");
    assert_eq(StandardRulesEngine::num_available_moves(state), 2, "King can't run to only two places");

    state.insert_piece(Position(4, 5), RED_CHARIOT);
    deny(StandardRulesEngine::is_king_in_check(state, RED), "King scared of own chariot");

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
    deny(StandardRulesEngine::is_legal_move(state, bad_move), "Code suffers from the 'you can't take my general who's in check because you're in check' problem");
}

BOOST_AUTO_TEST_CASE( scoring ) {
    auto state = StandardGameState(RED);
    auto ally_king_position = Position(2, 5);
    auto chariot_position = Position(5, 5);
    auto enemy_king_position = Position(8, 5);
    state.insert_piece(enemy_king_position, BLACK_GENERAL);
    state.insert_piece(chariot_position,    RED_CHARIOT);
    state.insert_piece(ally_king_position,  RED_GENERAL);
    assert_eq(piece_score(state), piece_value(RED_CHARIOT), "Generals don't cancel out 1");
    state.apply_move(Move(chariot_position, enemy_king_position));
    assert_eq(piece_score(state), -(piece_value(RED_CHARIOT) + piece_value(RED_GENERAL)), "Generals don't cancel out 2");
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
    assert_eq(ai_move, best, "AI chose a terrible move");
}
