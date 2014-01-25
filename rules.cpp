#include "rules.hpp"
#include <algorithm>
#include <stdexcept>

GameState empty_state();
set<Move> available_moves_for_piece(GameState& state, Position position, Piece piece);
set<Move> available_moves_for_general(GameState& state, Position position, Player owner);
set<Move> available_moves_for_advisor(GameState& state, Position position, Player owner);
set<Move> available_moves_for_elephant(GameState& state, Position position, Player owner);
set<Move> available_moves_for_horse(GameState& state, Position position, Player owner);
set<Move> available_moves_for_chariot(GameState& state, Position position, Player owner);
set<Move> available_moves_for_cannon(GameState& state, Position position, Player owner);
set<Move> available_moves_for_soldier(GameState& state, Position position, Player owner);
Position  mkPosition(int rank, int file);
Piece     mkPiece(PieceType type, Player owner);


set<Move> available_moves_for_general(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_advisor(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_elephant(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_horse(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_chariot(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_canon(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_soldier(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  return ret;
}

set<Move> available_moves_for_piece(GameState& state, Position position, Piece piece) {
  switch (piece.type) {
  case GENERAL:
    available_moves_for_general(state, position, piece.owner);
    break;
  case ADVISOR:
    available_moves_for_advisor(state, position, piece.owner);
    break;
  case ELEPHANT:
    available_moves_for_elephant(state, position, piece.owner);
    break;
  case HORSE:
    available_moves_for_horse(state, position, piece.owner);
    break;
  case CHARIOT:
    available_moves_for_chariot(state, position, piece.owner);
    break;
  case CANON:
    available_moves_for_canon(state, position, piece.owner);
    break;
  case SOLDIER:
    available_moves_for_soldier(state, position, piece.owner);
    break;
  default:
    throw logic_error("Unknown piece");
  }
}

set<Move> available_moves(GameState & state, Player player) {
  auto ret = set<Move>();
  auto& pieces = state.pieces;
  for_each(pieces.begin(), pieces.end(), [&] (pair<Position, Piece> piece) {
      auto new_moves = available_moves_for_piece(state, piece.first, piece.second);
      ret.insert(new_moves.begin(), new_moves.end());
  });
  return ret;
}

GameState new_game() {
  
}

void apply_move(GameState & state, Move move) {
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

bool Position::operator<(const Position& b) const {
  return (rank < b.rank) ||
          (rank == b.rank && file < b.file);
}

bool Position::operator==(const Position& b) const {
  return (rank == b.rank) && (file == b.file);
}

bool Move::operator<(const Move& b) const {
  return (from < b.from) ||
          (from == b.from && to < b.to);
}
