#include "rules.hpp"
#include "unimplemented.hpp"
#include "direction.hpp"
#include <algorithm>
#include <boost/multi_array.hpp>
#include <functional>
#include <list>
#include <boost/optional/optional.hpp>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace boost;

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
      ((1 <= rank && rank <= 3) ||
       (7 <= rank && rank <= 10));
}

bool should_flip_direction(Player player) {
  return player != RED;
}

bool is_position_occupied(const GameState& state, Position position) {
  return state.pieces.find(position) != state.pieces.end();
}

void for_each_piece(const GameState& state, function<void(Position, Piece)> action) {
    for_each(state.pieces.begin(), state.pieces.end(), [&] (const pair<Position, Piece> pair) {
        action(pair.first, pair.second);
    });
}

void insert_available_moves_for_general(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      all_moves.push_back(mkMove(position, direction));
  });
}

void insert_available_moves_for_advisor(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      all_moves.push_back(mkMove(position, direction));
  });
}

void insert_available_moves_for_horse(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      Position one_step = move_direction(position, direction);
      if (is_position_occupied(state, one_step))
        return;
      
      all_moves.push_back(
          mkMove(
              position,
              move_direction(
                  one_step,
                  rotate_left (direction))));
      all_moves.push_back(
          mkMove(
              position,
              move_direction(
                  one_step,
                  rotate_right(direction))));
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

bool has_crossed_river(const Position& position, Player player) {
    return player == RED
        ? (6 <= position.rank && position.rank <= 10)
        : (1 <= position.rank && position.rank <= 5);
}

void insert_available_moves_for_soldier(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
    auto northlike_direction = NORTH;
    if (should_flip_direction(owner))
        northlike_direction = SOUTH;
    all_moves.push_back(mkMove(position, northlike_direction));
    if (has_crossed_river(position, owner)) {
        all_moves.push_back(mkMove(position, EAST));
        all_moves.push_back(mkMove(position, WEST));
    }
}

void insert_available_moves_for_piece(const GameState& state, Position position, Piece piece, vector<Move>& all_moves) {
  switch (piece.piece_type) {
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
    throw logic_error("Unknown piece" + to_string(piece.piece_type));
  }
}

vector<Position> filter_pieces(const GameState& state, function<bool(Position, Piece)> pred) {
    auto matches = vector<Position>();
    for_each(state.pieces.begin(), state.pieces.end(), [&] (const pair<Position, Piece> pair) {
        if (pred(pair.first, pair.second))
            matches.push_back(pair.first);
    });
    return matches;
}

vector<Position> filter_pieces_by_type(const GameState& state, PieceType type) {
    return filter_pieces(state, [&] (const Position& position, const Piece& piece) {
        return piece.piece_type == type;
    });
}

optional<Position> shoot_ray_in_direction_until_collision(const GameState& state, const Position& center, Direction direction) {
    optional<Position> ret;
    shoot_ray_in_direction_until_should_stop(center, direction, [&] (const Position& candidate) {
        if (!is_position_valid(candidate))
            return true;
        if (is_position_occupied(state, candidate)) {
            ret = candidate;
            return true;
        }
        return false;
    });
    return ret;
}

optional<Piece> shoot_ray_in_direction_until_piece(const GameState& state, const Position& center, Direction direction) {
    auto maybe_position = shoot_ray_in_direction_until_collision(state, center, direction);
    if (!maybe_position)
        return optional<Piece>();
    return state.pieces.at(*maybe_position);
}

bool violates_flying_kings_rule(const GameState& state) {
    auto kings = filter_pieces_by_type(state, GENERAL);
    if (kings.size() < 2)
        return false;
    auto start = kings[0];
    auto is_king_in_direction = [&] (Direction direction) -> bool {
        auto piece_in_direction = shoot_ray_in_direction_until_piece(state, start, direction);
        if (!piece_in_direction)
            return false;
        else
            return (*piece_in_direction).piece_type == GENERAL;
    };
    return is_king_in_direction(NORTH)
        || is_king_in_direction(SOUTH);
};

bool violates_kings_in_castle_rule(const GameState& state) {
    bool any_kings_escaping = false;
    for_each_piece(state, [&] (const Position& position, const Piece& piece) {
        if (piece.piece_type != GENERAL)
            return;
        if (is_position_in_castle(position))
            return;
        any_kings_escaping = true;
    });
    return any_kings_escaping;
};

bool violates_can_only_capture_enemy_pieces_rule(const GameState& state, const Move& move) {
    auto from_piece_iter = state.pieces.find(move.from);
    auto captured_piece_iter = state.pieces.find(move.to);
    auto not_found = state.pieces.end();

    if (from_piece_iter == not_found)
      throw logic_error("Move does not have a piece on the 'from' coordinate");
    if (captured_piece_iter == not_found)
      return false;

    auto from_piece = (*from_piece_iter).second;
    auto captured_piece = (*captured_piece_iter).second;
    return from_piece.owner == captured_piece.owner;
}

bool is_invalid_state(const GameState& state) {
    return
        violates_flying_kings_rule(state) ||
        violates_kings_in_castle_rule(state);
}

void filter_invalid_moves(const GameState& state, vector<Move>& moves) {
  auto results_in_invalid_state = [&] (const Move& move) -> bool {
      return peek_move<bool>(state, move, is_invalid_state);
  };
  auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
      if (!is_position_valid(move.from))
          return true;
      if (!is_position_valid(move.to))
          return true;
      if (violates_can_only_capture_enemy_pieces_rule(state, move))
          return true;
      if (results_in_invalid_state(move))
          return true;
      return false;
  });
  moves.erase(new_end, moves.end());
}

vector<Move> available_moves(const GameState & state) {
  auto player = state.current_turn;
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
    auto state = mkState(RED);
    auto fill_home_rank = [&] (int rank, Player player) {
        insert_piece(state, mkPosition(rank, 1), mkPiece(CHARIOT, player));
        insert_piece(state, mkPosition(rank, 2), mkPiece(HORSE,   player));
        insert_piece(state, mkPosition(rank, 3), mkPiece(ELEPHANT,player));
        insert_piece(state, mkPosition(rank, 4), mkPiece(ADVISOR, player));
        insert_piece(state, mkPosition(rank, 5), mkPiece(GENERAL, player));
        insert_piece(state, mkPosition(rank, 6), mkPiece(ADVISOR, player));
        insert_piece(state, mkPosition(rank, 7), mkPiece(ELEPHANT,player));
        insert_piece(state, mkPosition(rank, 8), mkPiece(HORSE,   player));
        insert_piece(state, mkPosition(rank, 9), mkPiece(CHARIOT, player));
    };
    auto fill_cannons = [&] (int rank, Player player) {
        insert_piece(state, mkPosition(rank, 2), mkPiece(CANNON, player));
        insert_piece(state, mkPosition(rank, 8), mkPiece(CANNON, player));
    };
    auto fill_soldiers = [&] (int rank, Player player) {
        insert_piece(state, mkPosition(rank, 1), mkPiece(SOLDIER, player));
        insert_piece(state, mkPosition(rank, 3), mkPiece(SOLDIER, player));
        insert_piece(state, mkPosition(rank, 5), mkPiece(SOLDIER, player));
        insert_piece(state, mkPosition(rank, 7), mkPiece(SOLDIER, player));
        insert_piece(state, mkPosition(rank, 9), mkPiece(SOLDIER, player));
    };
    fill_home_rank(1, RED);
    fill_cannons  (3, RED);
    fill_soldiers (4, RED);
    fill_soldiers (7, BLACK);
    fill_cannons  (8, BLACK);
    fill_home_rank(10, BLACK);
    return state;
}

template<typename T> T peek_move(const GameState& state, Move move, const function<T(const GameState &)>& action) {
    auto scratch = state;
    apply_move(scratch, move);
    return action(scratch);
}

Player next_player(Player player) {
    return player == RED
        ? BLACK
        : RED;
}

void apply_move(GameState & state, const Move& move) {
    auto i = state.pieces.find(move.from);
    if (i == state.pieces.end())
        throw logic_error("No piece in the 'from' coordinate of this move");

    state.pieces[move.to] = state.pieces[move.from];
    state.pieces.erase(i);
    state.current_turn = next_player(state.current_turn);
}

int num_available_moves(const GameState& state) {
    return available_moves(state).size();
}

vector<Move> available_captures(const GameState& state) {
    auto moves = available_moves(state);
    auto captures = vector<Move>();
    for_each(moves.begin(), moves.end(), [&] (const Move& move) {
        if (is_capture(state, move))
            captures.push_back(move);
    });
    return captures;
}

bool is_capture(const GameState& state, const Move& move) {
    return !(state.pieces.find(move.to) == state.pieces.end());
}

int num_available_captures(const GameState& state) {
    return available_captures(state).size();
}

GameState mkState(Player current_turn) {
  auto ret = GameState();
  ret.pieces = map<Position, Piece>();
  ret.current_turn = current_turn;
  return ret;
}

void insert_piece(GameState& state, const Position& position, const Piece& piece) {
    state.pieces.insert(
        pair<Position, Piece>(
            position, piece));
}

multi_array<Piece, 2> state_to_board(const GameState& state) {
    multi_array<Piece, 2> ret(extents[10][9]);
    for_each_piece(state, [&] (const Position& position, Piece piece) {
        ret[position.rank-1][position.file-1] = piece;
    });
    return ret;
}

char character_for_piece(Piece piece) {
    switch (piece.piece_type) {
    case EMPTY:
        return '.';
    case GENERAL:
        return piece.owner == RED
            ? 'G'
            : 'g';
    case ADVISOR:
        return piece.owner == RED
            ? 'A'
            : 'a';
    case ELEPHANT:
        return piece.owner == RED
            ? 'E'
            : 'e';
    case HORSE:
        return piece.owner == RED
            ? 'H'
            : 'h';
    case CHARIOT:
        return piece.owner == RED
            ? 'R'
            : 'r';
    case CANNON:
        return piece.owner == RED
            ? 'N'
            : 'n';
    case SOLDIER:
        return piece.owner == RED
            ? 'S'
            : 's';
    default:
        throw logic_error("Unknown piece");
    }
    throw logic_error("????");
}

void for_range(int n, function<void(int)> action) {
    for (int i = 0; i != n; i++)
        action(i);
}

void repeat(int n, function<void()> action) {
    for_range(n, [&] (int x) { action(); });
}

string player_repr(Player player) {
    if (player == RED)
        return "Red";
    else
        return "Black";
}

void print_board(const GameState& state) {
    multi_array<Piece, 2> board = state_to_board(state);

    auto draw_river = [] () {
        repeat(8, [] () { cout << "~~"; });
        cout << "~" << endl;
    };

    auto draw_rank = [&] (int rank) {
        for_range(9, [&] (int j) {
            cout << character_for_piece(board[rank][j]) << ' ';
        });
        cout << endl;
    };

    cout << "Current Turn: " << player_repr(state.current_turn) << endl;
    for_range(10, [&] (int i) {
        i = 9 - i; // Flip the board
        if (i == 5)
            draw_river();
        draw_rank(i);
    });
}

void print_available_moves(const GameState& state) {
    auto moves = available_moves(state);
    for_each(moves.begin(), moves.end(), [] (const Move& move) {
        cout << move << endl;
    });
}

Piece mkPiece(PieceType piece_type, Player owner) {
  auto ret = Piece();
  ret.piece_type = piece_type;
  ret.owner = owner;
  return ret;
}

bool Move::operator<(const Move& b) const {
  return (from < b.from) ||
          (from == b.from && to < b.to);
}
