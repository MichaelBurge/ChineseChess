#include "../direction.hpp"
#include "../position.hpp"
#include "../rules.hpp"
#include "../test.hpp"
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
    if (ENABLE_VISUAL_TESTS)
        test_example_board();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what() << endl;
  }
}
