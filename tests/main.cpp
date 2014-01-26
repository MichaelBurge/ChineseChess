#include "../rules.hpp"
#include "../test.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

bool test_general() {
  auto generalPosition = mkPosition(2, 5);
  auto generalPiece = mkPiece(GENERAL, RED);

  auto state = GameState();
  state.current_turn = RED;
  state.pieces = map<Position, Piece>();
  state.pieces.insert(pair<Position, Piece>(generalPosition, generalPiece));

  auto moves = available_moves_for_piece(state, generalPosition, generalPiece);
  assert_eq(moves.size(), 4, "Incorrect number of moves 1");

  auto all_moves = available_moves(state, RED);
  assert_eq(moves.size(), 5, "Incorrect number of moves 2");
}
int main() {
  try {
    test_general();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what();
  }
}
