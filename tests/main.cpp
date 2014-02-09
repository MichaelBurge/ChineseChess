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

bool ENABLE_VISUAL_TESTS = false;

Position center_of_castle() { return mkPosition(2, 5); }
Position center_of_board() { return mkPosition(5, 5); }

void test_general() {
  auto state = mkState(RED);
  insert_piece(state, center_of_castle(), mkPiece(GENERAL, RED));

  assert_eq(num_available_moves(state), 4, "Incorrect number of general moves 3");
}

void test_advisor() {
  auto state = mkState(RED);
  insert_piece(state, center_of_castle(), mkPiece(ADVISOR, RED));

  assert_eq(num_available_moves(state), 4, "Incorrect number of advisor moves");

  with_90_degree_rotations(SOUTHEAST, [] (Direction direction) {
      auto state = mkState(RED);
      insert_piece(state, move_direction(center_of_castle(), direction), mkPiece(ADVISOR, RED));
      assert_eq(num_available_moves(state), 1, "Advisor not stuck on edge");
  });
}

void test_horse() {
  auto horsePosition = center_of_board();
  auto state = mkState(RED);
  insert_piece(state, horsePosition, mkPiece(HORSE, RED));

  assert_eq(num_available_moves(state), 8, "Incorrect number of horse moves 1");

  insert_piece(
      state,
      move_direction(horsePosition, NORTH),
      mkPiece(SOLDIER, BLACK));
  assert_eq(num_available_moves(state), 6, "Horse's leg can't be tripped");
}

void test_elephant() {
  auto elephantPosition = center_of_board();
  auto state = mkState(RED);
  insert_piece(state, elephantPosition, mkPiece(ELEPHANT, RED));

  assert_eq(num_available_moves(state), 4, "Incorrect number of elephant moves");
  insert_piece(
      state,
      move_direction(elephantPosition, NORTHEAST),
      mkPiece(SOLDIER, BLACK));
  assert_eq(num_available_moves(state), 3, "Can't block the elephant's eye");
}

void test_chariot() {
  auto chariotPosition = center_of_board();
  auto state = mkState(RED);
  insert_piece(state, chariotPosition, mkPiece(CHARIOT, RED));

  assert_eq(num_available_moves(state), 5+4+4+4, "Incorrect number of chariot moves 1");

  insert_piece(
      state,
      move_direction(chariotPosition, NORTH),
      mkPiece(SOLDIER, BLACK));
  assert_eq(num_available_moves(state), 1+4+4+4, "Incorrect number of chariot moves 2");

  insert_piece(
      state,
      move_direction(chariotPosition, WEST),
      mkPiece(SOLDIER, BLACK));
  assert_eq(num_available_moves(state), 1+1+4+4, "Incorrect number of chariot moves 3");
}

void test_cannon() {
  auto cannonPosition = center_of_board();
  auto state = mkState(RED);
  insert_piece(state, cannonPosition, mkPiece(CANNON, RED));

  assert_eq(num_available_moves(state), 5+4+4+4, "Incorrect number of chariot moves(no obstructions)");

  insert_piece(
      state,
      move_direction(cannonPosition, NORTH),
      mkPiece(SOLDIER, BLACK));
  assert_eq(num_available_moves(state), 0+4+4+4, "Incorrect number of chariot moves(one obstruction)");

  insert_piece(
      state,
      move_direction(move_direction(cannonPosition, NORTH), NORTH),
      mkPiece(ELEPHANT, BLACK));
  assert_eq(num_available_moves(state), 1+4+4+4, "Incorrect number of chariot moves(one capture)");

  //
  state = mkState(RED);
  insert_piece(state, mkPosition(10, 8), mkPiece(CANNON, RED));
  insert_piece(state, mkPosition(9, 8), mkPiece(ELEPHANT, BLACK));
  insert_piece(state, mkPosition(8, 8), mkPiece(ADVISOR, BLACK));
  insert_piece(state, mkPosition(7, 8), mkPiece(GENERAL, BLACK));
  deny(is_legal_move(state, mkMove(mkPosition(10, 8), mkPosition(7, 8))), "Double-hopping bug");
}

void test_soldier() {
    auto position = center_of_board();
    auto state = mkState(RED);
    insert_piece(state, position, mkPiece(SOLDIER, RED));

    assert_eq(num_available_moves(state), 1, "Incorrect number of pre-river soldier moves");

    state = mkState(RED);
    insert_piece(
        state,
        move_direction(position, NORTH),
        mkPiece(SOLDIER, RED));

    assert_eq(num_available_moves(state), 3, "Incorrect number of post-river soldier moves");
}

void test_piece_capture() {
    auto position = center_of_board();
    auto piece = mkPiece(CHARIOT, RED);
    auto ally  = mkPiece(CHARIOT, RED);
    auto enemy = mkPiece(CHARIOT, BLACK);

    auto state = mkState(RED);
    insert_piece(state, position, piece);
    assert_eq(num_available_captures(state), 0, "Captures incorrectly found 1");

    insert_piece(
        state,
        move_direction(position, NORTH),
        ally);
    assert_eq(num_available_captures(state), 0, "Allies can be captured");

    insert_piece(
        state,
        move_direction(position, WEST),
        enemy);
    assert_eq(num_available_captures(state), 1, "Capture move is not recorded properly");
}

void test_flying_kings_rule() {
    auto state = mkState(RED);
    insert_piece(state, mkPosition(2, 5), mkPiece(GENERAL, RED));
    assert_eq(num_available_moves(state), 4, "Incorrect # of king moves");

    insert_piece(state, mkPosition(8, 6), mkPiece(GENERAL, BLACK));
    assert_eq(num_available_moves(state), 3, "Enemy king doesn't block a move");


    insert_piece(state, mkPosition(5, 6), mkPiece(SOLDIER, BLACK));
    assert_eq(num_available_moves(state), 4, "Soldier doesn't block flying kings");
}

void test_example_board() {
    auto state = new_game();
    print_board(state);
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
    assert_eq((*m1).first, mkMove(e8_position, mkPosition(7, 5)), "E8E7 parsed incorrectly");

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
    auto state = mkState(RED);
    deny(winner(state), "Winner when no kings");

    insert_piece(state, mkPosition(2, 5), mkPiece(GENERAL, RED));
    _assert(winner(state), "Should be winner if one king");

    insert_piece(state, mkPosition(8, 6), mkPiece(GENERAL, BLACK));
    deny(winner(state), "Winner when two kings");
}

void test_check() {
    auto state = mkState(RED);
    insert_piece(state, mkPosition(2, 5), mkPiece(GENERAL, RED));
    assert_eq(num_available_moves(state), 4, "Incorrect number of general moves");
    insert_piece(state, mkPosition(6, 5), mkPiece(CHARIOT, BLACK));
    _assert(is_king_in_check(state, RED), "King not scared of chariot");

    insert_piece(state, mkPosition(4, 5), mkPiece(CHARIOT, RED));
    deny(is_king_in_check(state, RED), "King scared of own chariot");
}

void test_scoring() {
    auto state = mkState(RED);
    auto ally_king_position = mkPosition(2, 5);
    auto chariot_position = mkPosition(5, 5);
    auto enemy_king_position = mkPosition(8, 5);
    insert_piece(state, enemy_king_position, mkPiece(GENERAL, BLACK));
    insert_piece(state, chariot_position, mkPiece(CHARIOT, RED));
    insert_piece(state, ally_king_position, mkPiece(GENERAL, RED));
    assert_eq(piece_score(state), piece_value(CHARIOT), "Generals don't cancel out");
    apply_move(state, mkMove(chariot_position, enemy_king_position));
    assert_eq(piece_score(state), -(piece_value(CHARIOT) + piece_value(GENERAL)), "Generals don't cancel out");
}

void test_basic_ai() {
    auto test_captures_lone_general = [] (Player player, bool debug = false) {
        auto other_player = next_player(player);
	auto state = mkState(player);
	auto ally_king_position = mkPosition(2, 5);
	auto enemy_derp_position = mkPosition(3, 3);
	auto chariot_position = mkPosition(5, 5);
	auto enemy_king_position = mkPosition(8, 5);
      
	insert_piece(state, enemy_king_position, mkPiece(GENERAL, other_player));
	insert_piece(state, enemy_derp_position, mkPiece(CHARIOT, other_player));
	insert_piece(state, chariot_position, mkPiece(CHARIOT, player));
	insert_piece(state, ally_king_position, mkPiece(GENERAL, player));
	
	auto ai_move = best_move(state, 3, 1000, piece_score);
	if (debug) {
	    print_board(state);
	    cout << "Piece score: " << piece_score(state) << endl;
	    print_move_scores(move_scores(state, piece_score));
	    cout << "Move: " << ai_move << endl;
	}
	assert_eq(
          ai_move,
	  mkMove(chariot_position, enemy_king_position),
	  "AI chose a terrible move"
	);
    };

    test_captures_lone_general(RED, true);
    test_captures_lone_general(BLACK, true);
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
	count +=
	  n == 0
	    ? 1
	    : peek_move<int>(initial, move, false, [&] (const GameState& new_state) {
	        return perft(new_state, n - 1);
            });
    }
    return count;
}

void test_perft() {
    auto state = new_game();
    assert_eq(perft(state, 0), 44, "Failed perft(0)");
    assert_eq(perft(state, 1), 1920, "Failed perft(1)");
    //    assert_eq(perft(state, 2), 79666, "Failed perft(2)");
}

int main() {
  try {
    test_general();
    test_advisor();
    test_horse();
    test_elephant();
    test_chariot();
    test_cannon();
    test_soldier();
    test_piece_capture();
    test_flying_kings_rule();
    test_perft();
    test_parsing();
    test_winning();
    test_check();
    test_scoring();
    test_basic_ai();
    if (ENABLE_VISUAL_TESTS)
        test_example_board();
    test_performance();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what() << endl;
  }
}
