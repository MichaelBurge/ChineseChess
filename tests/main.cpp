#include "../direction.hpp"
#include "../position.hpp"
#include "../rules.hpp"
#include "../test.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

GameState state_with_one_piece(Position position, Piece piece) {
  auto state = GameState();
  state.current_turn = RED;
  state.pieces = map<Position, Piece>();
  state.pieces.insert(pair<Position, Piece>(position, piece));
  return state;
}

Position center_of_castle() { return mkPosition(2, 5); }
Position center_of_board() { return mkPosition(5, 5); }

void test_general() {
  auto generalPosition = center_of_castle();
  auto generalPiece = mkPiece(GENERAL, RED);

  auto state = state_with_one_piece(generalPosition, generalPiece);

  assert_eq(available_moves(state, RED).size(), 4, "Incorrect number of general moves 3");
}

void test_advisor() {
  auto advisorPosition = center_of_castle();
  auto advisorPiece = mkPiece(ADVISOR, RED);

  auto state = state_with_one_piece(advisorPosition, advisorPiece);

  auto moves = available_moves(state, RED);
  assert_eq(moves.size(), 4, "Incorrect number of advisor moves");
}

void test_horse() {
  auto horsePosition = center_of_board();
  auto horsePiece = mkPiece(HORSE, RED);

  auto state = state_with_one_piece(horsePosition, horsePiece);

  assert_eq(available_moves(state, RED).size(), 8, "Incorrect number of horse moves 1");

  state.pieces.insert(pair<Position, Piece>(
      move_direction(horsePosition, NORTH),
      mkPiece(SOLDIER, BLACK)));

  assert_eq(available_moves(state, RED).size(), 6, "Horse's leg can't be tripped");
}

void test_elephant() {
  auto elephantPosition = center_of_board();
  auto elephantPiece = mkPiece(ELEPHANT, RED);

  auto state = state_with_one_piece(elephantPosition, elephantPiece);

  assert_eq(available_moves(state, RED).size(), 4, "Incorrect number of elephant moves");
  state.pieces.insert(pair<Position, Piece>(
      move_direction(elephantPosition, NORTHEAST),
      mkPiece(SOLDIER, BLACK)));
  assert_eq(available_moves(state, RED).size(), 3, "Can't block the elephant's eye");
}

void test_chariot() {
  auto chariotPosition = center_of_board();
  auto chariotPiece = mkPiece(CHARIOT, RED);

  auto state = state_with_one_piece(chariotPosition, chariotPiece);

  assert_eq(available_moves(state, RED).size(), 5+4+4+4, "Incorrect number of chariot moves 1");

  state.pieces.insert(pair<Position, Piece>(
      move_direction(chariotPosition, NORTH),
      mkPiece(SOLDIER, BLACK)));
  assert_eq(available_moves(state, RED).size(), 1+4+4+4, "Incorrect number of chariot moves 2");

  state.pieces.insert(pair<Position, Piece>(
      move_direction(chariotPosition, WEST),
      mkPiece(SOLDIER, BLACK)));
  assert_eq(available_moves(state, RED).size(), 1+1+4+4, "Incorrect number of chariot moves 3");
}

void test_cannon() {
  auto cannonPosition = center_of_board();
  auto cannonPiece = mkPiece(CANNON, RED);

  auto state = state_with_one_piece(cannonPosition, cannonPiece);

  assert_eq(available_moves(state, RED).size(), 5+4+4+4, "Incorrect number of chariot moves(no obstructions)");

  state.pieces.insert(
      pair<Position, Piece>(
          move_direction(cannonPosition, NORTH),
          mkPiece(SOLDIER, BLACK)));
  assert_eq(available_moves(state, RED).size(), 0+4+4+4, "Incorrect number of chariot moves(one obstruction)");

  state.pieces.insert(
      pair<Position, Piece>(
          move_direction(move_direction(cannonPosition, NORTH), NORTH),
          mkPiece(ELEPHANT, BLACK)));
  assert_eq(available_moves(state, RED).size(), 1+4+4+4, "Incorrect number of chariot moves(one capture)");
}

void test_piece_capture() {
}

int main() {
  try {
    test_general();
    test_advisor();
    test_horse();
    test_elephant();
    test_chariot();
    test_cannon();
    test_piece_capture();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what() << endl;
  }
}
