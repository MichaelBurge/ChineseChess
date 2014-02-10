#include "rules.hpp"
#include "exceptions.hpp"
#include "direction.hpp"
#include <algorithm>
#include <boost/multi_array.hpp>
#include <functional>
#include <list>
#include <boost/lexical_cast.hpp>
#include <boost/optional/optional.hpp>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace std::placeholders;
using namespace boost;

GameState::GameState(Player _current_turn) {
  this->pieces = map<Position, Piece>();
  this->current_turn = _current_turn;
  this->undo_stack = stack<UndoNode, list<UndoNode> >();
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
            return true;
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
    throw logic_error("Unknown piece " + lexical_cast<string>(piece.piece_type));
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

bool violates_pieces_stuck_in_castle_rule(const GameState& state) {
    bool any_pieces_escaping = false;
    for_each_piece(state, [&] (const Position& position, const Piece& piece) {
        if (!( piece.piece_type == GENERAL || piece.piece_type == ADVISOR ))
            return;
        if (is_position_in_castle(position))
            return;
        any_pieces_escaping = true;
    });
    return any_pieces_escaping;
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

vector<Move> filter_available_captures(const GameState& state, function<bool(const Move&)> pred) {
    return filter_available_moves(state, [&] (const Move& move) {
        return is_capture(state, move) && pred(move);
    });
}

vector<Move> captures_for_position(const GameState& state, const Position& position) {
    return filter_available_captures(state, [&] (const Move& move) {
        return move.to == position;
    });
}

bool is_king_in_check(const GameState& state, Player player) {
    GameState switchedTurnState = state;
    switchedTurnState.current_turn = next_player(player);
    auto kings = filter_pieces_by_type(switchedTurnState, GENERAL);
    bool in_check = false;
    for_each(kings.begin(), kings.end(), [&] (const Position& position) {
        auto _pair = *(state.pieces.find(position));
        auto owner = _pair.second.owner;
        if (owner != player)
            return;
        auto captures = captures_for_position(switchedTurnState, position);
        if (!captures.empty())
            in_check = true;
    });
    return in_check;
}

bool is_invalid_state(const GameState& state) {
    return
        violates_flying_kings_rule(state) ||
        violates_pieces_stuck_in_castle_rule(state);
}

void filter_invalid_moves(const GameState& state, vector<Move>& moves) {
  auto results_in_invalid_state = [&] (const Move& move) -> bool {
      return peek_move<bool>(state, move, false, is_invalid_state);
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

vector<Move> available_moves_without_check(const GameState & state) {
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

vector<Move> available_moves(const GameState & state) {
    auto results_in_check = [&] ( const Move& move) -> bool {
	return peek_move<bool>(state, move, false, bind(&is_king_in_check, _1, state.current_turn));
    };
    vector<Move> moves = available_moves_without_check(state);
    auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
	    if (results_in_check(move))
	    return true;
         return false;
    });
    moves.erase(new_end, moves.end());
    return moves;
}

GameState new_game() {
    auto state = GameState(RED);
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
template<typename T> T peek_move(const GameState& state, Move move, bool check_legality, const function<T(const GameState &)>& action) {
    auto scratch = state;
    apply_move(scratch, move, check_legality);
    return action(scratch);
}

template int peek_move<int>(const GameState&, Move, bool, const function<int(const GameState &)>&);

template<> void peek_move(const GameState& state, Move move, bool check_legality, const function<void(const GameState &)>& action) {
    auto scratch = state;
    apply_move(scratch, move, check_legality);
    action(scratch);
}

Player next_player(Player player) {
    return player == RED
        ? BLACK
        : RED;
}

bool is_legal_move(const GameState& state, const Move& move, bool allow_check) {
    vector<Move> moves =
	allow_check
	  ? available_moves_without_check(state)
          : available_moves(state);
    return !(std::find(moves.begin(), moves.end(), move) == moves.end());
}

void apply_move(GameState & state, const Move& move, bool check_legality) {
    if (check_legality && !is_legal_move(state, move, true))
        throw illegal_move(move);    
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

vector<Move> filter_available_moves(const GameState& state, function<bool(const Move&)> pred) {
    auto moves = available_moves_without_check(state);
    auto matches = vector<Move>();
    for_each(moves.begin(), moves.end(), [&] (const Move& move) {
        if (pred(move))
            matches.push_back(move);
    });
    return matches;
}    

vector<Move> available_moves_from(const GameState& state, const Position& position) {
    return filter_available_moves(state, [&] (const Move& move) {
        return move.from == position;
    });
}

vector<Move> available_captures(const GameState& state) {
    return filter_available_moves(state, [&] (const Move& move) {
        return is_capture(state, move);
    });
}

bool is_capture(const GameState& state, const Move& move) {
    return !(state.pieces.find(move.to) == state.pieces.end());
}

int num_available_captures(const GameState& state) {
    return available_captures(state).size();
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
    auto c = '\a';
    switch (piece.piece_type) {
    case EMPTY:
        c = '.'; break;
    case GENERAL:
        c = 'g'; break;
    case ADVISOR:
        c = 'a'; break;
    case ELEPHANT:
        c = 'e'; break;
    case HORSE:
        c = 'h'; break;
    case CHARIOT:
        c = 'r'; break;
    case CANNON:
        c = 'n'; break;
    case SOLDIER:
        c = 's'; break;
    default:
        throw logic_error("Unknown piece");
    }
    if (piece.owner == RED)
        c = toupper(c);
    return c;
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
        if (i == 4)
            draw_river();
        draw_rank(i);
    });
}

optional<Player> winner(const GameState& state) {
    auto kings = filter_pieces_by_type(state, GENERAL);
    if (kings.size() == 2 || kings.size() == 0)
        return optional<Player>();
    return (*(state.pieces.find(kings[0]))).second.owner;
}

void print_available_moves(const GameState& state) {
    print_moves(available_moves(state));
}

Piece mkPiece(PieceType piece_type, Player owner) {
  auto ret = Piece();
  ret.piece_type = piece_type;
  ret.owner = owner;
  return ret;
}
