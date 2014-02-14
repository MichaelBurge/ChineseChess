#include "reference.hpp"
#include "../direction.hpp"
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

ReferenceRules THE_REFERENCE_RULES = ReferenceRules();

GameState ReferenceRules::new_game() {
    auto state = GameState(RED);
    auto fill_home_rank = [&] (int rank, Player player) {
        state.insert_piece(Position(rank, 1), Piece(CHARIOT, player, 0));
        state.insert_piece(Position(rank, 2), Piece(HORSE,   player, 0));
        state.insert_piece(Position(rank, 3), Piece(ELEPHANT,player, 0));
        state.insert_piece(Position(rank, 4), Piece(ADVISOR, player, 0));
        state.insert_piece(Position(rank, 5), Piece(GENERAL, player));
        state.insert_piece(Position(rank, 6), Piece(ADVISOR, player, 1));
        state.insert_piece(Position(rank, 7), Piece(ELEPHANT,player, 1));
        state.insert_piece(Position(rank, 8), Piece(HORSE,   player, 1));
        state.insert_piece(Position(rank, 9), Piece(CHARIOT, player, 1));
    };
    auto fill_cannons = [&] (int rank, Player player) {
        state.insert_piece(Position(rank, 2), Piece(CANNON, player, 0));
        state.insert_piece(Position(rank, 8), Piece(CANNON, player, 1));
    };
    auto fill_soldiers = [&] (int rank, Player player) {
        state.insert_piece(Position(rank, 1), Piece(SOLDIER, player, 0));
        state.insert_piece(Position(rank, 3), Piece(SOLDIER, player, 1));
        state.insert_piece(Position(rank, 5), Piece(SOLDIER, player, 2));
        state.insert_piece(Position(rank, 7), Piece(SOLDIER, player, 3));
        state.insert_piece(Position(rank, 9), Piece(SOLDIER, player, 4));
    };
    fill_home_rank(1, RED);
    fill_cannons  (3, RED);
    fill_soldiers (4, RED);
    fill_soldiers (7, BLACK);
    fill_cannons  (8, BLACK);
    fill_home_rank(10, BLACK);
    return state;
}

void ReferenceRules::insert_available_moves_for_general(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      all_moves.push_back(Move(position, direction));
  });
}

void ReferenceRules::insert_available_moves_for_advisor(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      all_moves.push_back(Move(position, direction));
  });
}

void ReferenceRules::insert_available_moves_for_horse(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
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

void ReferenceRules::insert_available_moves_for_elephant(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
      Position one_step = move_direction(position, direction);
      if (is_position_occupied(state, one_step))
        return;

      all_moves.push_back(Move(position, move_direction(one_step, direction)));
  });
}

void ReferenceRules::insert_available_moves_for_chariot(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      shoot_ray_in_direction_until_should_stop(position, direction, [&] (const Position& candidate) {
          if (!candidate.is_valid())
	      return true;
          all_moves.push_back(Move(position, candidate));

          if (is_position_occupied(state, candidate))
	      return true;
          return false;
      });
  });
}

void ReferenceRules::insert_available_moves_for_cannon(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
  with_90_degree_rotations(NORTH, [&] (Direction direction) {
      bool has_collided = false;
      shoot_ray_in_direction_until_should_stop(position, direction, [&] (const Position& candidate) {
          if (!candidate.is_valid())
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

bool ReferenceRules::has_crossed_river(const Position& position, Player player) {
    return player == RED
        ? (6 <= position.rank && position.rank <= 10)
        : (1 <= position.rank && position.rank <= 5);
}

void ReferenceRules::insert_available_moves_for_soldier(const GameState& state, Position position, Player owner, vector<Move>& all_moves) {
    auto should_flip_direction = [] (Player player) {
	return player != RED;
    };

    auto northlike_direction = NORTH;
    if (should_flip_direction(owner))
        northlike_direction = SOUTH;
    all_moves.push_back(Move(position, northlike_direction));
    if (has_crossed_river(position, owner)) {
        all_moves.push_back(Move(position, EAST));
        all_moves.push_back(Move(position, WEST));
    }
}

void ReferenceRules::insert_available_moves_for_piece(const GameState& state, Position position, Piece piece, vector<Move>& all_moves) {
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

optional<Position> ReferenceRules::shoot_ray_in_direction_until_collision(const GameState& state, const Position& center, Direction direction) {
    optional<Position> ret;
    shoot_ray_in_direction_until_should_stop(center, direction, [&] (const Position& candidate) {
	if (!candidate.is_valid())
            return true;
        if (is_position_occupied(state, candidate)) {
            ret = candidate;
            return true;
        }
        return false;
    });
    return ret;
}

optional<Piece> ReferenceRules::shoot_ray_in_direction_until_piece(const GameState& state, const Position& center, Direction direction) {
    auto maybe_position = shoot_ray_in_direction_until_collision(state, center, direction);
    if (!maybe_position)
        return optional<Piece>();
    return state.get_piece(*maybe_position);
}

bool ReferenceRules::violates_flying_kings_rule(const GameState& state) {
    auto red_king = state.get_position(Piece(GENERAL, RED));
    auto black_king = state.get_position(Piece(GENERAL, BLACK));

    if (!red_king || !black_king)
        return false;
    auto start = *red_king;
    auto is_king_in_direction = [&] (Direction direction) -> bool {
        auto piece_in_direction = shoot_ray_in_direction_until_piece(state, start, direction);
        if (!piece_in_direction)
            return false;
        else
            return (*piece_in_direction).piece_type == GENERAL;
    };
    return is_king_in_direction(NORTH);
};

bool ReferenceRules::violates_pieces_stuck_in_castle_rule(const GameState& state) {
    vector<Piece> pieces_to_check = {
	Piece(GENERAL, RED),
	Piece(ADVISOR, RED, 0),
	Piece(ADVISOR, RED, 1),
	Piece(GENERAL, BLACK),
	Piece(ADVISOR, BLACK, 0),
	Piece(ADVISOR, BLACK, 1)
    };
    for (const Piece& piece : pieces_to_check) {
	auto position = state.get_position(piece);
	if (!position)
	    continue;
	if (is_position_in_castle(*position))
	    continue;
	return true;
    }
    return false;
};

bool ReferenceRules::violates_can_only_capture_enemy_pieces_rule(const GameState& state, const Move& move) {
    auto from_piece = state.get_piece(move.from);
    auto captured_piece = state.get_piece(move.to);

    if (!from_piece)
	throw logic_error("Move " + move_repr(move) + " does not have a piece on the 'from' coordinate");
    if (!captured_piece)
	return false;

    return (*from_piece).owner == (*captured_piece).owner;
}

bool ReferenceRules::is_king_in_check(const GameState& original_state, Player player) {
    auto state = original_state;
    state.current_turn(next_player(player));
    auto king = state.get_position(Piece(GENERAL, player));
    if (!king)
	return false;
    
    return !captures_for_position(state, *king).empty();
}

bool ReferenceRules::is_invalid_state(const GameState& state) {
    return
        violates_flying_kings_rule(state) ||
        violates_pieces_stuck_in_castle_rule(state);
}


bool ReferenceRules::is_position_in_castle(const Position& position) {
    const auto& rank = position.rank, file = position.file;
    return
	4 <= file && file <= 6 &&
	((1 <= rank && rank <= 3) ||
	 (7 <= rank && rank <= 10));
}

bool ReferenceRules::is_position_occupied(const GameState& state, Position position) {
    return !!(state.get_piece(position));
}

void ReferenceRules::filter_invalid_moves(const GameState& state, vector<Move>& moves) {
  auto results_in_invalid_state = [&] (const Move& move) -> bool {
      return state.peek_move<bool>(move, [&] (const GameState& newState) {
	  return this->is_invalid_state(newState);
      });
  };
  auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
      if (!move.from.is_valid())
          return true;
      if (!move.to.is_valid())
          return true;
      if (violates_can_only_capture_enemy_pieces_rule(state, move))
          return true;
      if (results_in_invalid_state(move))
          return true;
      return false;
  });
  moves.erase(new_end, moves.end());
}

vector<Move> ReferenceRules::available_moves_without_check(const GameState & state) {
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

vector<Move> ReferenceRules::available_moves(const GameState & state) {
    vector<Move> moves = available_moves_without_check(state);
    auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
	    if (results_in_check(state, move))
	    return true;
         return false;
    });
    moves.erase(new_end, moves.end());
    return moves;
}

bool ReferenceRules::is_legal_move(const GameState& state, const Move& move, bool allow_check) {
    vector<Move> moves =
	allow_check
	  ? available_moves_without_check(state)
          : available_moves(state);
    return !(std::find(moves.begin(), moves.end(), move) == moves.end());
}

optional<Player> ReferenceRules::winner(const GameState& state) {
    auto red_king = state.get_position(Piece(GENERAL, RED));
    auto black_king = state.get_position(Piece(GENERAL, BLACK));
    if ((!!red_king && !!black_king) || (!red_king && !black_king)) {
	return optional<Player>();
    }
    if (!red_king)
	return BLACK;
    return RED;
}

void ReferenceRules::print_available_moves(const GameState& state) {
    print_moves(available_moves(state));
}

bool ReferenceRules::is_capture(const GameState& state, const Move& move) {
    return !!state.get_piece(move.to);
}
