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
#include <iostream>
#include <stdexcept>
#include <iomanip>
using namespace std;

Position center_of_castle() { return Position(2, 5); }
Position center_of_board() { return Position(5, 5); }

void test_uint128() {
    uint128_t derp = uint128_t(0x00F0FFFF0000FFFF, 0x0F00FFFF0000FFFF);

    assert_eq(lsb_first_set(derp), (uint8_t)0, "First one wrong 1");
    assert_eq(msb_first_set(derp), (uint8_t)119, "Second one wrong 1");

    uint128_t herp = uint128_t(0x00000000FFF00000, 0x0000000000000000);
    assert_eq(lsb_first_set(herp), (uint8_t)83, "First one wrong 2");
    assert_eq(msb_first_set(herp), (uint8_t)95, "Second one wrong 2");

    uint128_t zerp = uint128_t(0x0000000000000000, 0x00000000FFF00000);
    assert_eq(lsb_first_set(zerp), (uint8_t)20, "First one wrong 3");
    assert_eq(msb_first_set(zerp), (uint8_t)31, "Second one wrong 3");

}

void test_general() {
  auto state = StandardGameState(RED);
  state.insert_piece(center_of_castle(), RED_GENERAL);

  assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect number of general moves 3");
}

void test_advisor() {
  auto state = StandardGameState(RED);
  state.insert_piece(center_of_castle(), RED_ADVISOR);

  assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect number of advisor moves");

  with_90_degree_rotations(SOUTHEAST, [] (Direction direction) {
      auto state = StandardGameState(RED);
      state.insert_piece(move_direction(center_of_castle(), direction), RED_ADVISOR);
      assert_eq(StandardRulesEngine::num_available_moves(state), 1, "Advisor not stuck on edge");
  });
}

void test_horse() {
  auto horsePosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(horsePosition, RED_HORSE);

  assert_eq(StandardRulesEngine::num_available_moves(state), 8, "Incorrect number of horse moves 1");

  state.insert_piece(
      move_direction(horsePosition, NORTH),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 6, "Horse's leg can't be tripped");
}

void test_elephant() {
  auto elephantPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(elephantPosition, RED_ELEPHANT);

  assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect number of elephant moves");
  state.insert_piece(
      move_direction(elephantPosition, NORTHEAST),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 3, "Can't block the elephant's eye");
}

void test_chariot() {
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

void test_cannon() {
  auto cannonPosition = center_of_board();
  auto state = StandardGameState(RED);
  state.insert_piece(cannonPosition, RED_CANNON);

  assert_eq(StandardRulesEngine::num_available_moves(state), 5+4+4+4, "Incorrect number of chariot moves(no obstructions)");

  state.insert_piece(
      move_direction(cannonPosition, NORTH),
      BLACK_SOLDIER);
  assert_eq(StandardRulesEngine::num_available_moves(state), 0+4+4+4, "Incorrect number of chariot moves(one obstruction)");

  state.insert_piece(
      move_direction(move_direction(cannonPosition, NORTH), NORTH),
      BLACK_ELEPHANT);
  assert_eq(StandardRulesEngine::num_available_moves(state), 1+4+4+4, "Incorrect number of chariot moves(one capture)");

  state = StandardGameState(RED);
  state.insert_piece(Position(10, 8), RED_CANNON);
  state.insert_piece(Position(9, 8), BLACK_ELEPHANT);
  state.insert_piece(Position(8, 8), BLACK_ADVISOR);
  state.insert_piece(Position(7, 8), BLACK_GENERAL);
  deny(StandardRulesEngine::is_legal_move(state, Move(Position(10, 8), Position(7, 8))), "Double-hopping bug");
}

void test_soldier() {
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

void test_piece_capture() {
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

void test_flying_kings_rule() {
    auto state = StandardGameState(RED);
    state.insert_piece(Position(2, 5), RED_GENERAL);
    assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Incorrect # of king moves");

    state.insert_piece(Position(8, 6), BLACK_GENERAL);
    assert_eq(StandardRulesEngine::num_available_moves(state), 3, "Enemy king doesn't block a move");

    state.insert_piece(Position(5, 6), BLACK_SOLDIER);
    assert_eq(StandardRulesEngine::num_available_moves(state), 4, "Soldier doesn't block flying kings");
}

void test_parsing() {
    auto c1 = parse_value<char>("103");
    _assert(c1, "103 doesn't parse");
    assert_eq((*c1).first, '1', "1 doesn't parse correctly");
    assert_eq((*c1).second, string("03"), "03 isn't the remaining text");

    auto i1 = parse_value<int>("0");
    assert_eq((*i1).first, 0, "0 doesn't parse correctly");

    auto i2 = parse_value<int>("103");
    _assert(i2, "103 doesn't parse");
    assert_eq((*i2).first, 103, "103 doesn't parse correctly");

    auto u1 = parse_until("1234", [] (char c) {
	    return c != '1' && c != '2';
	});
    _assert(u1, "1234 doesn't parse at all");
    assert_eq((*u1).first, string("12"), "12 not parsed");
    assert_eq((*u1).second, string("34"), "34 not parsed");

    auto u2 = parse_until("derp", [] (char) { return true; });
    deny(u2, "u2 parsed");

    auto r1 = parse_rank("5");
    _assert(r1, "5 doesn't parse as a rank");

    auto r2 = parse_rank("10");
    _assert(r2, "10 doesn't parse as a rank");

    auto r3 = parse_rank("0");
    deny(r3, "0 shouldn't parse as a rank");

    auto f1 = parse_file("A");
    _assert(f1, "A doesn't parse as a file");

    auto f2 = parse_file("J");
    deny(f2, "J shouldn't parse as a file");

    auto e8_position = Position(8, 5);

    auto p1 = parse_position("E8");
    _assert(p1, "E8 doesn't parse as a position");
    assert_eq((*p1).first, e8_position, "E8 parsed incorrectly");

    auto p2 = parse_position("derp");
    deny(p2, "'derp' parsed as a position");

    auto m1 = parse_move("e8E7");
    _assert(m1, "E8E7 doesn't parse as a move");
    assert_eq((*m1).first, Move(e8_position, Position(7, 5)), "E8E7 parsed incorrectly");

    auto m2 = parse_move("derp");
    deny(m2, "'derp' parsed as a move");

    auto tk1 = parse_token("Herp|Derp", '|');
    _assert(tk1, "Herp|Derp doesn't split correctly");
    assert_eq((*tk1).first, string("Herp"), "Herp not parsed correctly");
    assert_eq((*tk1).second, string("Derp"), "Incorrect remaining text");

    auto tk2 = parse_token("move E1E8", ' ');
    _assert(tk2, "move E1E8 doesn't split correctly");
    assert_eq((*tk2).first, string("move"), "move not parsed correctly");
    assert_eq((*tk2).second, string("E1E8"), "E1E8 not parsed correctly");
}

void test_winning() {
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

void test_game_state_dictionary_storage() {
    auto x = ReferenceGameStateDictionaryStorage();
    x.insert_piece(Position(2, 5), RED_CHARIOT);
    x.insert_piece(Position(2, 5), RED_CHARIOT);
    assert_eq(x.size(), 1, "Insert two elements");
    x.insert_piece(Position(2, 6), BLACK_CHARIOT);
}

void test_data_structures() {
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

void test_check() {
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

void test_scoring() {
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

bool debug;

void test_basic_minimax() {
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

void test_performance() {
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

void test_perft() {
    auto state = StandardGameState::new_game();
    assert_eq(perft(state, 0), 44, "Failed perft(0)");
    assert_eq(perft(state, 1), 1920, "Failed perft(1)");
    assert_eq(perft(state, 2), 79666, "Failed perft(2)");
}

int main() {
  try {
      test_uint128();
      test_game_state_dictionary_storage();
      test_data_structures();
      test_general();
      test_advisor();
      test_horse();
      test_elephant();
      test_chariot();
      test_cannon();
      test_soldier();
      test_piece_capture();
      test_flying_kings_rule();
      test_parsing();
      test_winning();
      test_check();
      test_scoring();
      test_basic_minimax();
      test_performance();
      test_perft();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what() << endl;
  }
}
