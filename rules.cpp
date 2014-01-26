#include "rules.hpp"
#include "unimplemented.hpp"
#include "direction.hpp"
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <sstream>
using namespace std;

// Prototypes

GameState empty_state();
set<Move> available_moves_for_piece(GameState& state, Position position, Piece piece);
set<Move> available_moves_for_general(GameState& state, Position position, Player owner);
set<Move> available_moves_for_advisor(GameState& state, Position position, Player owner);
set<Move> available_moves_for_elephant(GameState& state, Position position, Player owner);
set<Move> available_moves_for_horse(GameState& state, Position position, Player owner);
set<Move> available_moves_for_chariot(GameState& state, Position position, Player owner);
set<Move> available_moves_for_cannon(GameState& state, Position position, Player owner);
set<Move> available_moves_for_soldier(GameState& state, Position position, Player owner);
void      filter_invalid_moves(GameState& state, set<Move>& moves);
bool      is_position_in_castle(const Position& position);
bool      should_flip_direction(Direction direction);
Move      mkMove(const Position& from, const Position& to);
Piece     mkPiece(PieceType type, Player owner);

// Implementation

template<typename T> bool between(const T& x, const T& a, const T& b) {
  return a < x && x < b;
}

bool is_position_in_castle(const Position& position) {
  const auto& rank = position.rank, file = position.file;
  return
    4 <= file && file <= 6 &&
    1 <= rank && rank <= 3;
}

bool should_flip_direction(Player player) {
  return player != RED;
}

bool is_position_occupied(GameState& state, Position position) {
  return state.pieces.find(position) != state.pieces.end();
}

set<Move> available_moves_for_general(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      ret.insert(mkMove(position, move_direction(position, direction)));
  });
  return ret;
}

set<Move> available_moves_for_advisor(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      ret.insert(mkMove(position, move_direction(position, direction)));
  });
  return ret;
}

set<Move> available_moves_for_horse(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      Position one_step = move_direction(position, direction);
      if (is_position_occupied(state, one_step))
        return;
      
      ret.insert(mkMove(position, move_direction(one_step, rotate_left (direction))));
      ret.insert(mkMove(position, move_direction(one_step, rotate_right(direction))));
  });
  return ret;
}

set<Move> available_moves_for_elephant(GameState& state, Position position, Player owner) {
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
  switch (piece.pieceType) {
  case GENERAL:
    return available_moves_for_general(state, position, piece.owner);
    break;
  case ADVISOR:
    return available_moves_for_advisor(state, position, piece.owner);
    break;
  case ELEPHANT:
    return available_moves_for_elephant(state, position, piece.owner);
    break;
  case HORSE:
    return available_moves_for_horse(state, position, piece.owner);
    break;
  case CHARIOT:
    return available_moves_for_chariot(state, position, piece.owner);
    break;
  case CANON:
    return available_moves_for_canon(state, position, piece.owner);
    break;
  case SOLDIER:
    return available_moves_for_soldier(state, position, piece.owner);
    break;
  default:
    throw logic_error("Unknown piece" + to_string(piece.pieceType));
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
  throw unimplemented("new_game()");
}

void apply_move(GameState & state, Move move) {
}

GameState empty_state() {
  auto ret = GameState();
  ret.pieces = map<Position, Piece>();
  ret.current_turn = RED;
  return ret;
}

Move mkMove(const Position& from, const Position& to) {
  auto ret = Move();
  ret.from = from;
  ret.to = to;
  return ret;
}

Piece mkPiece(PieceType pieceType, Player owner) {
  auto ret = Piece();
  ret.pieceType = pieceType;
  ret.owner = owner;
  return ret;
}

bool Move::operator<(const Move& b) const {
  return (from < b.from) ||
          (from == b.from && to < b.to);
}

string to_string(int n) {
  ostringstream ss;
  ss << n;
  return ss.str();
}
