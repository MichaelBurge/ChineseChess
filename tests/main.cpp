#include "../rules.hpp"
#include "../test.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

void test_general() {
  auto generalPosition = mkPosition(2, 5);
  auto generalPiece = mkPiece(GENERAL, RED);

  auto state = GameState();
  state.current_turn = RED;
  state.pieces = map<Position, Piece>();
  state.pieces.insert(pair<Position, Piece>(generalPosition, generalPiece));

  auto moves1 = available_moves_for_general(state, generalPosition, RED);
  cerr << moves1.size() << " size " << endl;
  assert_eq(moves1.size(), 4, "Incorrect number of moves 1");

  auto moves2 = available_moves_for_piece(state, generalPosition, generalPiece);
  cerr << moves2.size() << " size " << endl;
  assert_eq(moves2.size(), 4, "Incorrect number of moves 2");

  auto moves3 = available_moves(state, RED);
  assert_eq(moves3.size(), 4, "Incorrect number of moves 3");
}
int main() {
  try {
    test_general();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what();
  }
}
