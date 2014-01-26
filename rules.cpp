#include "rules.hpp"
#include "unimplemented.hpp"
#include <algorithm>
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
Direction opposite_direction(Direction direction);
Position  move_direction(const Position& position, Direction direction, Player player);
Move      mkMove(const Position& from, const Position& to);
Position  mkPosition(int rank, int file);
Piece     mkPiece(PieceType type, Player owner);
//string    to_string(int n);

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

Direction opposite_direction(Direction direction) {
  switch (direction) {
  case NORTH:
    return SOUTH;
  case SOUTH:
    return NORTH;
  case EAST:
    return WEST;
  case WEST:
    return EAST;
  case SOUTHEAST:
    return NORTHWEST;
  case NORTHWEST:
    return SOUTHEAST;
  case SOUTHWEST:
    return NORTHEAST;
  case NORTHEAST:
    return SOUTHWEST;
  default:
    throw logic_error("Unknown direction");
  }
}

bool should_flip_direction(Player player) {
  return player != RED;
}

Position move_direction(const Position& position, Direction direction, Player player) {
  // Players have north/south flipped since they sit on opposite ends
  direction =
    should_flip_direction(player)
    ? opposite_direction(direction)
    : direction
    ;
  switch (direction) {
  case NORTH:
    return mkPosition(position.rank + 1, position.file);
  case SOUTH:
    return mkPosition(position.rank - 1, position.file);
  case WEST:
    return mkPosition(position.rank, position.file + 1);
  case EAST:
    return mkPosition(position.rank, position.file - 1);
  case SOUTHEAST:
    return mkPosition(position.rank - 1, position.file - 1);
  case SOUTHWEST:
    return mkPosition(position.rank - 1, position.file + 1);
  case NORTHEAST:
    return mkPosition(position.rank + 1, position.file - 1);
  case NORTHWEST:
    return mkPosition(position.rank + 1, position.file + 1);
  default:
    throw logic_error("Unknown direction");
  }
}

set<Move> available_moves_for_general(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  ret.insert(mkMove(position, move_direction(position, NORTH, owner)));
  ret.insert(mkMove(position, move_direction(position, SOUTH, owner)));
  ret.insert(mkMove(position, move_direction(position, WEST,  owner)));
  ret.insert(mkMove(position, move_direction(position, EAST,  owner)));
  return ret;
}

set<Move> available_moves_for_advisor(GameState& state, Position position, Player owner) {
  auto ret = set<Move>();
  ret.insert(mkMove(position, move_direction(position, NORTHEAST, owner)));
  ret.insert(mkMove(position, move_direction(position, SOUTHEAST, owner)));
  ret.insert(mkMove(position, move_direction(position, NORTHWEST,  owner)));
  ret.insert(mkMove(position, move_direction(position, SOUTHWEST,  owner)));
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

Position mkPosition(int rank, int file) {
  auto position = Position();
  position.rank = rank;
  position.file = file;
  return position;
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

string to_string(int n) {
  ostringstream ss;
  ss << n;
  return ss.str();
}
