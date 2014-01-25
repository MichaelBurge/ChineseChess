#include "rules.hpp"

GameState empty_state();
Position  mkPosition(int rank, int file);
Piece     mkPiece(PieceType type, Player owner);

set<Move> available_moves(GameState & state, Player player) {
  auto ret = set<Move>();
  return ret;
}

GameState new_game() {
  
}

GameState empty_state() {
  auto ret = GameState();
  ret.pieces = map<Position, Piece>();
  ret.current_turn = 0;
  return ret;
}

Position mkPosition(int rank, int file) {
  auto position = Position();
  position.rank = rank;
  position.file = file;
  return position;
}
