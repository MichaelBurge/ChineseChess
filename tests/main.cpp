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

  auto moves1 = available_moves_for_general(state, generalPosition, RED);
  assert_eq(moves1.size(), 4, "Incorrect number of general moves 1");

  auto moves2 = available_moves_for_piece(state, generalPosition, generalPiece);
  assert_eq(moves2.size(), 4, "Incorrect number of general moves 2");

  auto moves3 = available_moves(state, RED);
  assert_eq(moves3.size(), 4, "Incorrect number of general moves 3");
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

int main() {
  try {
    test_general();
    test_advisor();
    test_horse();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what();
  }
}
