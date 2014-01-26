#include "rules.hpp"
#include "unimplemented.hpp"
#include "direction.hpp"
#include <algorithm>
#include <functional>
#include <list>
#include <stdexcept>
#include <string>
#include <sstream>
using namespace std;

template<typename T> bool between(const T& x, const T& a, const T& b) {
  return a < x && x < b;
}

bool is_position_valid(const Position& position) {
  const auto& rank = position.rank, file = position.file;
  return
    1 <= file && file <= 9 &&
    1 <= rank && rank <= 10;
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

bool is_position_occupied(const GameState& state, Position position) {
  return state.pieces.find(position) != state.pieces.end();
}

void insert_available_moves_for_general(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      all_moves.push_back(mkMove(position, move_direction(position, direction)));
  });
}

void insert_available_moves_for_advisor(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      all_moves.push_back(mkMove(position, move_direction(position, direction)));
  });
}

void insert_available_moves_for_horse(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      Position one_step = move_direction(position, direction);
      if (is_position_occupied(state, one_step))
        return;
      
      all_moves.push_back(mkMove(position, move_direction(one_step, rotate_left (direction))));
      all_moves.push_back(mkMove(position, move_direction(one_step, rotate_right(direction))));
  });
}

void insert_available_moves_for_elephant(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      Position one_step = move_direction(position, direction);
      if (is_position_occupied(state, one_step))
        return;

      all_moves.push_back(mkMove(position, move_direction(one_step, direction)));
  });
}

void insert_available_moves_for_chariot(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      shoot_ray_in_direction_until_should_stop(position, direction, [&] (const Position& candidate) {
          if (!is_position_valid(candidate))
            return true;
          all_moves.push_back(mkMove(position, candidate));

          if (is_position_occupied(state, candidate))
            return true;
          return false;
      });
  });
}

void insert_available_moves_for_cannon(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      bool has_collided = false;
      shoot_ray_in_direction_until_should_stop(position, direction, [&] (const Position& candidate) {
          if (!is_position_valid(candidate))
            return true;
          if (!has_collided) {
            if (is_position_occupied(state, candidate)) {
              has_collided = true;
              return false;
            }
            all_moves.push_back(mkMove(position, candidate));
          } else {
            if (!is_position_occupied(state, candidate))
              return false;
            all_moves.push_back(mkMove(position, candidate));
          }
          return false;
      });
  });
}

void insert_available_moves_for_soldier(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  throw unimplemented("Write the soldier!");
}

void insert_available_moves_for_piece(const GameState& state, Position position, Piece piece, vector<Move>& all_moves) {
  switch (piece.pieceType) {
  case GENERAL:
    insert_available_moves_for_general(state, position, piece.owner, all_moves);
    break;
  case ADVISOR:
    insert_available_moves_for_advisor(state, position, piece.owner, all_moves);
    break;
  case ELEPHANT:
    insert_available_moves_for_elephant(state, position, piece.owner, all_moves);
    break;
  case HORSE:
    insert_available_moves_for_horse(state, position, piece.owner, all_moves);
    break;
  case CHARIOT:
    insert_available_moves_for_chariot(state, position, piece.owner, all_moves);
    break;
  case CANNON:
    insert_available_moves_for_cannon(state, position, piece.owner, all_moves);
    break;
  case SOLDIER:
    insert_available_moves_for_soldier(state, position, piece.owner, all_moves);
    break;
  default:
    throw logic_error("Unknown piece" + to_string(piece.pieceType));
  }
}

void filter_invalid_moves(const GameState& state, vector<Move>& moves) {
  auto is_enemy_piece = [&] (const Move& move) -> bool {
    auto from_piece_iter = state.pieces.find(move.from);
    auto captured_piece_iter = state.pieces.find(move.to);
    auto not_found = state.pieces.end();

    if (from_piece_iter == not_found)
      throw logic_error("Move does not have a piece on the 'from' coordinate");
    if (captured_piece_iter == not_found)
      return true;

    auto from_piece = (*from_piece_iter).second;
    auto captured_piece = (*captured_piece_iter).second;
    return from_piece.owner == captured_piece.owner;
  };
  remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
      return
        is_position_valid(move.from) &&
        is_position_valid(move.to) &&
        is_enemy_piece(move)
        ;
  });
}

vector<Move> available_moves(const GameState & state, Player player) {
  auto all_moves = vector<Move>();
  auto& pieces = state.pieces;
  for_each(pieces.begin(), pieces.end(), [&] (pair<Position, Piece> piece) {
      if (piece.second.owner != player)
        return;
      insert_available_moves_for_piece(state, piece.first, piece.second, all_moves);
  });
  filter_invalid_moves(state, all_moves);
  return all_moves;
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
