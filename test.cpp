#include "rules.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
using namespace std;

void assert(bool condition, string message) {
  if (condition)
    return;
  cerr << "Failing test! Message: " << message << endl;
  throw logic_error("Assertion failure");
}

bool test_general() {
  auto state = GameState();
  state.current_turn = RED;
  state.pieces = map<Position, Piece>();
  state.pieces.insert(pair<Position, Piece>(mkPosition(2, 5), mkPiece(GENERAL, RED)));
  auto moves = available_moves(state, RED);
  assert(moves.size() == 5, "Incorrect number of moves");
}

int main() {
  try {
    test_general();
  } catch (logic_error& error) {
    cerr << "Aborted test: " << error.what();
  }
}
