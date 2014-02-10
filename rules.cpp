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
    return !!(state.get_piece(position));
}

void insert_available_moves_for_general(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      all_moves.push_back(Move(position, direction));
  });
}

void insert_available_moves_for_advisor(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      all_moves.push_back(Move(position, direction));
  });
}

void insert_available_moves_for_horse(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      Position one_step = move_direction(position, direction);
      if (is_position_occupied(state, one_step))
        return;
      
      all_moves.push_back(
          Move(
	      position,
              move_direction(
                  one_step,
                  rotate_left (direction))));
      all_moves.push_back(
          Move(
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

      all_moves.push_back(Move(position, move_direction(one_step, direction)));
  });
}

void insert_available_moves_for_chariot(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      shoot_ray_in_direction_until_should_stop(position, direction, [&] (const Position& candidate) {
          if (!is_position_valid(candidate))
            return true;
          all_moves.push_back(Move(position, candidate));

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
            all_moves.push_back(Move(position, candidate));
          } else {
            if (!is_position_occupied(state, candidate))
              return false;
            all_moves.push_back(Move(position, candidate));
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
    all_moves.push_back(Move(position, northlike_direction));
    if (has_crossed_river(position, owner)) {
        all_moves.push_back(Move(position, EAST));
        all_moves.push_back(Move(position, WEST));
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
    return state.get_piece(*maybe_position);
}

bool violates_flying_kings_rule(const GameState& state) {
    auto kings = state.filter_pieces_by_type(GENERAL);
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
    state.for_each_piece([&] (const Position& position, const Piece& piece) {
        if (!( piece.piece_type == GENERAL || piece.piece_type == ADVISOR ))
            return;
        if (is_position_in_castle(position))
            return;
        any_pieces_escaping = true;
    });
    return any_pieces_escaping;
};

bool violates_can_only_capture_enemy_pieces_rule(const GameState& state, const Move& move) {
    auto from_piece = state.get_piece(move.from);
    auto captured_piece = state.get_piece(move.to);

    if (!from_piece)
	throw logic_error("Move " + move_repr(move) + " does not have a piece on the 'from' coordinate");
    if (!captured_piece)
	return false;

    return (*from_piece).owner == (*captured_piece).owner;
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

bool is_king_in_check(const GameState& original_state, Player player) {
    auto state = original_state;
    state.current_turn(next_player(player));
    bool in_check = false;
    for (const Position& position : state.filter_pieces_by_type(GENERAL)) {
        auto piece = state.get_piece(position);
	if ((*piece).owner != player)
            continue;
        auto captures = captures_for_position(state, position);

        if (!captures.empty())
            in_check = true;
    };
    return in_check;
}

bool is_invalid_state(const GameState& state) {
    return
        violates_flying_kings_rule(state) ||
        violates_pieces_stuck_in_castle_rule(state);
}

void filter_invalid_moves(const GameState& state, vector<Move>& moves) {
  auto results_in_invalid_state = [&] (const Move& move) -> bool {
      return state.peek_move<bool>(move, is_invalid_state);
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
    auto player = state.current_turn();
    auto all_moves = vector<Move>();
    state.for_each_piece([&] (Position position, Piece piece) {
	if (piece.owner != player)
	    return;
	insert_available_moves_for_piece(state, position, piece, all_moves);
    });
    filter_invalid_moves(state, all_moves);
    return all_moves;
}

bool results_in_check(const GameState& state, const Move& move) {
    return state.peek_move<bool>(move, bind(&is_king_in_check, _1, state.current_turn()));
};

vector<Move> available_moves(const GameState & state) {
    vector<Move> moves = available_moves_without_check(state);
    auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
	    if (results_in_check(state, move))
	    return true;
         return false;
    });
    moves.erase(new_end, moves.end());
    return moves;
}

GameState new_game() {
    auto state = GameState(RED);
    auto fill_home_rank = [&] (int rank, Player player) {
        state.insert_piece(mkPosition(rank, 1), Piece(CHARIOT, player));
        state.insert_piece(mkPosition(rank, 2), Piece(HORSE,   player));
        state.insert_piece(mkPosition(rank, 3), Piece(ELEPHANT,player));
        state.insert_piece(mkPosition(rank, 4), Piece(ADVISOR, player));
        state.insert_piece(mkPosition(rank, 5), Piece(GENERAL, player));
        state.insert_piece(mkPosition(rank, 6), Piece(ADVISOR, player));
        state.insert_piece(mkPosition(rank, 7), Piece(ELEPHANT,player));
        state.insert_piece(mkPosition(rank, 8), Piece(HORSE,   player));
        state.insert_piece(mkPosition(rank, 9), Piece(CHARIOT, player));
    };
    auto fill_cannons = [&] (int rank, Player player) {
        state.insert_piece(mkPosition(rank, 2), Piece(CANNON, player));
        state.insert_piece(mkPosition(rank, 8), Piece(CANNON, player));
    };
    auto fill_soldiers = [&] (int rank, Player player) {
        state.insert_piece(mkPosition(rank, 1), Piece(SOLDIER, player));
        state.insert_piece(mkPosition(rank, 3), Piece(SOLDIER, player));
        state.insert_piece(mkPosition(rank, 5), Piece(SOLDIER, player));
        state.insert_piece(mkPosition(rank, 7), Piece(SOLDIER, player));
        state.insert_piece(mkPosition(rank, 9), Piece(SOLDIER, player));
    };
    fill_home_rank(1, RED);
    fill_cannons  (3, RED);
    fill_soldiers (4, RED);
    fill_soldiers (7, BLACK);
    fill_cannons  (8, BLACK);
    fill_home_rank(10, BLACK);
    return state;
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
    return !!(state.get_piece(move.to));
}

int num_available_captures(const GameState& state) {
    return available_captures(state).size();
}

optional<Player> winner(const GameState& state) {
    auto kings = state.filter_pieces_by_type(GENERAL);
    if (kings.size() == 2 || kings.size() == 0)
        return optional<Player>();
    return (*(state.get_piece(kings[0]))).owner;
}

void print_available_moves(const GameState& state) {
    print_moves(available_moves(state));
}
