#include "../direction.hpp"
#include "../parsing.hpp"
#include "../position.hpp"
#include "../rules.hpp"
#include "../test.hpp"
#include "../scoring.hpp"
#include "../minimax.hpp"
#include "../interpreter.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

Position center_of_castle() { return mkPosition(2, 5); }
Position center_of_board() { return mkPosition(5, 5); }

void test_general() {
  auto state = GameState(RED);
  state.insert_piece(center_of_castle(), Piece(GENERAL, RED));

  assert_eq(num_available_moves(state), 4, "Incorrect number of general moves 3");
}

void test_advisor() {
  auto state = GameState(RED);
  state.insert_piece(center_of_castle(), Piece(ADVISOR, RED, 0));

  assert_eq(num_available_moves(state), 4, "Incorrect number of advisor moves");

  with_90_degree_rotations(SOUTHEAST, [] (Direction direction) {
      auto state = GameState(RED);
      state.insert_piece(move_direction(center_of_castle(), direction), Piece(ADVISOR, RED, 0));
      assert_eq(num_available_moves(state), 1, "Advisor not stuck on edge");
  });
}

void test_horse() {
  auto horsePosition = center_of_board();
  auto state = GameState(RED);
  state.insert_piece(horsePosition, Piece(HORSE, RED, 0));

  assert_eq(num_available_moves(state), 8, "Incorrect number of horse moves 1");

  state.insert_piece(
      move_direction(horsePosition, NORTH),
      Piece(SOLDIER, BLACK, 0));
  assert_eq(num_available_moves(state), 6, "Horse's leg can't be tripped");
}

void test_elephant() {
  auto elephantPosition = center_of_board();
  auto state = GameState(RED);
  state.insert_piece(elephantPosition, Piece(ELEPHANT, RED, 0));

  assert_eq(num_available_moves(state), 4, "Incorrect number of elephant moves");
  state.insert_piece(
      move_direction(elephantPosition, NORTHEAST),
      Piece(SOLDIER, BLACK, 0));
  assert_eq(num_available_moves(state), 3, "Can't block the elephant's eye");
}

void test_chariot() {
  auto chariotPosition = center_of_board();
  auto state = GameState(RED);
  state.insert_piece(chariotPosition, Piece(CHARIOT, RED, 0));
  assert_eq(num_available_moves(state), 5+4+4+4, "Incorrect number of chariot moves 1");

  state.insert_piece(
      move_direction(chariotPosition, NORTH),
      Piece(SOLDIER, BLACK, 0));
  assert_eq(num_available_moves(state), 1+4+4+4, "Incorrect number of chariot moves 2");

  state.insert_piece(
      move_direction(chariotPosition, WEST),
      Piece(SOLDIER, BLACK, 1));
  assert_eq(num_available_moves(state), 1+1+4+4, "Incorrect number of chariot moves 3");
}

void test_cannon() {
  auto cannonPosition = center_of_board();
  auto state = GameState(RED);
  state.insert_piece(cannonPosition, Piece(CANNON, RED, 0));

  assert_eq(num_available_moves(state), 5+4+4+4, "Incorrect number of chariot moves(no obstructions)");

  state.insert_piece(
      move_direction(cannonPosition, NORTH),
      Piece(SOLDIER, BLACK, 0));
  assert_eq(num_available_moves(state), 0+4+4+4, "Incorrect number of chariot moves(one obstruction)");

  state.insert_piece(
      move_direction(move_direction(cannonPosition, NORTH), NORTH),
      Piece(ELEPHANT, BLACK, 1));
  assert_eq(num_available_moves(state), 1+4+4+4, "Incorrect number of chariot moves(one capture)");

  state = GameState(RED);
  state.insert_piece(mkPosition(10, 8), Piece(CANNON, RED, 0));
  state.insert_piece(mkPosition(9, 8), Piece(ELEPHANT, BLACK, 0));
  state.insert_piece(mkPosition(8, 8), Piece(ADVISOR, BLACK, 0));
  state.insert_piece(mkPosition(7, 8), Piece(GENERAL, BLACK, 0));
  deny(is_legal_move(state, Move(mkPosition(10, 8), mkPosition(7, 8))), "Double-hopping bug");
}

void test_soldier() {
    auto position = center_of_board();
    auto state = GameState(RED);
    state.insert_piece(position, Piece(SOLDIER, RED, 0));

    assert_eq(num_available_moves(state), 1, "Incorrect number of pre-river soldier moves");

    state = GameState(RED);
    state.insert_piece(
        move_direction(position, NORTH),
        Piece(SOLDIER, RED, 0));

    assert_eq(num_available_moves(state), 3, "Incorrect number of post-river soldier moves");
}

void test_piece_capture() {
    auto position = center_of_board();
    auto piece = Piece(CHARIOT, RED, 0);
    auto ally  = Piece(CHARIOT, RED, 1);
    auto enemy = Piece(CHARIOT, BLACK, 0);

    auto state = GameState(RED);
    state.insert_piece(position, piece);
    assert_eq(num_available_captures(state), 0, "Captures incorrectly found 1");

    state.insert_piece(
        move_direction(position, NORTH),
        ally);
    assert_eq(num_available_captures(state), 0, "Allies can be captured");

    state.insert_piece(
        move_direction(position, WEST),
        enemy);
    assert_eq(num_available_captures(state), 1, "Capture move is not recorded properly");
}

void test_flying_kings_rule() {
    auto state = GameState(RED);
    state.insert_piece(mkPosition(2, 5), Piece(GENERAL, RED));
    assert_eq(num_available_moves(state), 4, "Incorrect # of king moves");

    state.insert_piece(mkPosition(8, 6), Piece(GENERAL, BLACK));
    assert_eq(num_available_moves(state), 3, "Enemy king doesn't block a move");

    state.insert_piece(mkPosition(5, 6), Piece(SOLDIER, BLACK, 0));
    assert_eq(num_available_moves(state), 4, "Soldier doesn't block flying kings");
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

    auto u2 = parse_until("derp", [] (char c) { return true; });
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

    auto e8_position = mkPosition(8, 5);

    auto p1 = parse_position("E8");
    _assert(p1, "E8 doesn't parse as a position");
    assert_eq((*p1).first, e8_position, "E8 parsed incorrectly");

    auto p2 = parse_position("derp");
    deny(p2, "'derp' parsed as a position");

    auto m1 = parse_move("e8E7");
    _assert(m1, "E8E7 doesn't parse as a move");
    assert_eq((*m1).first, Move(e8_position, mkPosition(7, 5)), "E8E7 parsed incorrectly");

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
    auto state = GameState(RED);
    _assert(!winner(state), "Winner when no kings");

    state.insert_piece(mkPosition(2, 5), Piece(GENERAL, RED));
    _assert(winner(state), "Should be winner if one king");

    state.insert_piece(mkPosition(8, 6), Piece(GENERAL, BLACK));
    deny(winner(state), "Winner when two kings");
}

void test_game_state_dictionary_storage() {
    auto x = GameStateDictionaryStorage();
    x.insert_piece(mkPosition(2, 5), Piece(CHARIOT, RED, 0));
    x.insert_piece(mkPosition(2, 5), Piece(CHARIOT, RED, 1));
    assert_eq(x.size(), 1, "Insert two elements");
    x.insert_piece(mkPosition(2, 6), Piece(CHARIOT, BLACK, 0));
}

void test_data_structures() {
    auto game = new_game();
    auto num_pieces = 0;
    game.for_each_piece([&] (Position position, Piece piece) {
        num_pieces++;
    });
    assert_eq(num_pieces, 32, "for_each_piece is broken");

    auto state = GameState(RED);
    auto old_position = mkPosition(2, 5);
    auto new_position = move_direction(old_position, WEST);
    state.insert_piece(old_position, Piece(GENERAL, RED, 0));
    state.insert_piece(move_direction(old_position, NORTH), Piece(CHARIOT, RED, 0));
    state.insert_piece(mkPosition(9, 1), Piece(CHARIOT, BLACK, 0));
    state.peek_move<void>(Move(old_position, new_position), [&] (const GameState newState) {
	newState.peek_move<void>(Move(mkPosition(9, 1), mkPosition(9, 2)), [&] (const GameState& newState) {
	    newState.peek_move<void>(Move(new_position, old_position), [] (const GameState& newState) {
	    });
	});
    });
    assert_eq(state.current_turn(), RED, "current_turn not preserved 1");
    best_move(state, 3, 1000, piece_score);
    assert_eq(state.current_turn(), RED, "current_turn not preserved 2");

    auto general_position = state.get_position(Piece(GENERAL, RED));
    _assert(!!general_position, "No general");
    assert_eq(*general_position, old_position, "General in wrong place");

    auto red_chariot_position = state.get_position(Piece(CHARIOT, RED, 0));
    _assert(!!red_chariot_position, "No red chariot");
    assert_eq(*red_chariot_position, move_direction(old_position, NORTH), "Red chariot in wrong place");

    auto black_chariot_position = state.get_position(Piece(CHARIOT, BLACK, 0));
    _assert(!!black_chariot_position, "No black chariot");
    assert_eq(*black_chariot_position, mkPosition(9, 1), "Black chariot in wrong place");
}

void test_check() {
    auto state = GameState(RED);
    auto position = center_of_castle();
    state.insert_piece(position, Piece(GENERAL, RED));
    assert_eq(num_available_moves(state), 4, "Incorrect number of general moves");

    state.insert_piece(mkPosition(6, 5), Piece(CHARIOT, BLACK, 0));
    _assert(is_king_in_check(state, RED), "King not scared of chariot");
    _assert(results_in_check(state, Move(position, move_direction(position, NORTH))), "NORTH check");
    _assert(results_in_check(state, Move(position, move_direction(position, SOUTH))), "SOUTH check");
    deny(results_in_check(state, Move(position, move_direction(position, WEST))), "WEST check");
    deny(results_in_check(state, Move(position, move_direction(position, EAST))), "EAST check");
    assert_eq(num_available_moves(state), 2, "King can't run to only two places");

    state.insert_piece(mkPosition(4, 5), Piece(CHARIOT, RED, 0));
    deny(is_king_in_check(state, RED), "King scared of own chariot");

    state = GameState(BLACK);
    state.insert_piece(mkPosition(1, 5), Piece(GENERAL, RED, 0));
    state.insert_piece(mkPosition(1, 6), Piece(ADVISOR, RED, 0));
    state.insert_piece(mkPosition(10, 5), Piece(GENERAL, BLACK, 0));
    state.insert_piece(mkPosition(10, 6), Piece(CHARIOT, BLACK, 0));
    state.insert_piece(mkPosition(10, 7), Piece(CHARIOT, RED, 0));
    state.insert_piece(mkPosition(2, 5), Piece(SOLDIER, RED, 0));
    state.insert_piece(mkPosition(2, 1), Piece(CHARIOT, BLACK, 1));
    state.insert_piece(mkPosition(2, 7), Piece(HORSE, BLACK, 0));
    auto bad_move = Move(mkPosition(10,6), mkPosition(1,6));
    deny(is_legal_move(state, bad_move), "Code suffers from the 'you can't take my general who's in check because you're in check' problem");
}

void test_scoring() {
    auto state = GameState(RED);
    auto ally_king_position = mkPosition(2, 5);
    auto chariot_position = mkPosition(5, 5);
    auto enemy_king_position = mkPosition(8, 5);
    state.insert_piece(enemy_king_position, Piece(GENERAL, BLACK, 0));
    state.insert_piece(chariot_position, Piece(CHARIOT, RED, 0));
    state.insert_piece(ally_king_position, Piece(GENERAL, RED, 0));
    assert_eq(piece_score(state), piece_value(CHARIOT), "Generals don't cancel out 1");
    state.apply_move(Move(chariot_position, enemy_king_position));
    assert_eq(piece_score(state), -(piece_value(CHARIOT) + piece_value(GENERAL)), "Generals don't cancel out 2");
}

bool debug;

void test_basic_minimax() {
    auto test_captures_lone_general = [] (Player player) {
        auto other_player = next_player(player);
	auto state = GameState(player);
	auto ally_king_position = mkPosition(2, 5);
	auto enemy_derp_position = mkPosition(3, 3);
	auto chariot_position = mkPosition(5, 5);
	auto enemy_king_position = mkPosition(8, 4);
      
	state.insert_piece(enemy_king_position, Piece(GENERAL, other_player));
	state.insert_piece(enemy_derp_position, Piece(CHARIOT, other_player, 0));
	state.insert_piece(chariot_position,    Piece(CHARIOT, player, 0));
	state.insert_piece(ally_king_position,  Piece(GENERAL, player));

	auto best = Move(chariot_position, mkPosition(5, 4));
	auto ai_move = best_move(state, 3, 1000, piece_score);
	assert_eq(ai_move, best, "AI chose a terrible move");
    };

    test_captures_lone_general(RED);
    // Black king test isn't realistic because the red king can't get in the north castle
    //test_captures_lone_general(BLACK, true);
}

void test_performance() {
    auto interpreter = Interpreter();
    interpreter.max_nodes = 1000000;
    interpreter.difficulty = 2;
    interpreter.cmd_run_computer();
}

int perft(const GameState& initial, int n) {
    int count = 0;
    for (const Move& move : available_moves(initial)) {
	if (n == 0) {
	    count += 1;
	} else {
	    count += initial.peek_move<int>(move, [&] (const GameState& new_state) {
	        return perft(new_state, n - 1);
            });
	}
    }
    return count;
}

void test_perft() {
    auto state = new_game();
    assert_eq(perft(state, 0), 44, "Failed perft(0)");
    assert_eq(perft(state, 1), 1920, "Failed perft(1)");
    assert_eq(perft(state, 2), 79666, "Failed perft(2)");
}

int main() {
  try {
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
