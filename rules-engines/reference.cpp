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

namespace implementation {
    void insert_available_moves_for_general(const ReferenceGameState&, Position position, vector<Move>& all_moves) {
	with_90_degree_rotations(NORTH, [&] (Direction direction) {
		auto new_position = move_direction(position, direction);
		if (!is_position_in_castle(new_position))
		    return;
		all_moves.push_back(Move(position, new_position));
	    });
    }

    void insert_available_moves_for_advisor(const ReferenceGameState&, Position position, vector<Move>& all_moves) {
	with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
		auto new_position = move_direction(position, direction);
		if (!is_position_in_castle(new_position))
		    return;
		all_moves.push_back(Move(position, new_position));
	    });
    }

    void insert_available_moves_for_horse(const ReferenceGameState& state, Position position, vector<Move>& all_moves) {
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

    void insert_available_moves_for_elephant(const ReferenceGameState& state, Position position, vector<Move>& all_moves) {
	with_90_degree_rotations(NORTHEAST, [&] (Direction direction) {
		Position one_step = move_direction(position, direction);
		if (is_position_occupied(state, one_step))
		    return;

		all_moves.push_back(Move(position, move_direction(one_step, direction)));
	    });
    }

    void insert_available_moves_for_chariot(const ReferenceGameState& state, Position position, vector<Move>& all_moves) {
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

    void insert_available_moves_for_cannon(const ReferenceGameState& state, Position position, vector<Move>& all_moves) {
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

    bool has_crossed_river(const Position& position, Player player) {
	return player == RED
	    ? (6 <= position.rank && position.rank <= 10)
	    : (1 <= position.rank && position.rank <= 5);
    }

    void insert_available_moves_for_soldier(const ReferenceGameState&, Position position, Player owner, vector<Move>& all_moves) {
	all_moves.push_back(Move(position, owner == RED ? NORTH : SOUTH));
	if (has_crossed_river(position, owner)) {
	    all_moves.push_back(Move(position, EAST));
	    all_moves.push_back(Move(position, WEST));
	}
    }

    void insert_available_moves_for_piece(const ReferenceGameState& state, Position position, Piece piece, vector<Move>& all_moves) {
	switch (piece) {
	case RED_GENERAL:
	case BLACK_GENERAL:
	    insert_available_moves_for_general(state, position, all_moves);
	    break;
	case RED_ADVISOR:
	case BLACK_ADVISOR:
	    insert_available_moves_for_advisor(state, position, all_moves);
	    break;
	case RED_ELEPHANT:
	case BLACK_ELEPHANT:
	    insert_available_moves_for_elephant(state, position, all_moves);
	    break;
	case RED_HORSE:
	case BLACK_HORSE:
	    insert_available_moves_for_horse(state, position, all_moves);
	    break;
	case RED_CHARIOT:
	case BLACK_CHARIOT:
	    insert_available_moves_for_chariot(state, position, all_moves);
	    break;
	case RED_CANNON:
	case BLACK_CANNON:
	    insert_available_moves_for_cannon(state, position, all_moves);
	    break;
	case RED_SOLDIER:
	case BLACK_SOLDIER:
	    insert_available_moves_for_soldier(state, position, owner(piece), all_moves);
	    break;
	default:
	    throw logic_error("Unknown piece " + lexical_cast<string>((int)piece));
	}
    }

    optional<Position> shoot_ray_in_direction_until_collision(const ReferenceGameState& state, const Position& center, Direction direction) {
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

    optional<Piece> shoot_ray_in_direction_until_piece(const ReferenceGameState& state, const Position& center, Direction direction) {
	auto maybe_position = shoot_ray_in_direction_until_collision(state, center, direction);
	if (!maybe_position)
	    return optional<Piece>();
	return state.get_piece(*maybe_position);
    }

    bool violates_flying_kings_rule(const ReferenceGameState& state) {
	auto red_king = state.get_king_position(RED);
	auto black_king = state.get_king_position(BLACK);

	if (!red_king || !black_king)
	    return false;
	auto start = *red_king;
	auto is_king_in_direction = [&] (Direction direction) -> bool {
	    auto piece_in_direction = shoot_ray_in_direction_until_piece(state, start, direction);
	    if (!piece_in_direction)
		return false;
	    else
		return
		    *piece_in_direction == RED_GENERAL ||
		    *piece_in_direction == BLACK_GENERAL;
	};
	return is_king_in_direction(NORTH);
    };

    bool violates_can_only_capture_enemy_pieces_rule(const ReferenceGameState& state, const Move& move) {
	auto from_piece = state.get_piece(move.from);
	auto captured_piece = state.get_piece(move.to);

	if (!from_piece)
	    throw logic_error("Move " + move_repr(move) + " does not have a piece on the 'from' coordinate");
	if (!captured_piece)
	    return false;

	return owner(from_piece) == owner(captured_piece);
    }

    bool is_invalid_state(const ReferenceGameState& state) {
	return
	    violates_flying_kings_rule(state);
    }


    bool is_position_in_castle(const Position& position) {
	const auto& rank = position.rank, file = position.file;
	return
	    4 <= file && file <= 6 &&
	    ((1 <= rank && rank <= 3) ||
	     (7 <= rank && rank <= 10));
    }

    bool is_position_occupied(const ReferenceGameState& state, Position position) {
	return !position.is_valid() || state.get_piece(position);
    }

    void filter_invalid_moves(const ReferenceGameState& state, vector<Move>& moves) {
	auto results_in_invalid_state = [&] (const Move& move) -> bool {
	    bool x;
	    state.peek_move(move, [&] (const ReferenceGameState& newState) {
	        x = is_invalid_state(newState);
	    });
	    return x;
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


    optional<Player> __winner(const ReferenceGameState& state) {
	return (_num_available_moves(state) == 0)
	    ? next_player(state.current_turn())
	    : optional<Player>();
    }

    vector<Move> _available_moves_without_check(const ReferenceGameState & state) {
	auto player = state.current_turn();
	auto all_moves = vector<Move>();
	state.for_each_piece([&] (Position position, Piece piece) {
	    if (owner(piece) != player)
		return;
	    insert_available_moves_for_piece(state, position, piece, all_moves);
	});
	filter_invalid_moves(state, all_moves);
	return all_moves;
    }
    
    bool _results_in_check(const ReferenceGameState& state, const Move& move)
    {
	bool ret;
	state.peek_move(move, [&] (const ReferenceGameState& new_state) {
	    ret = _is_king_in_check(new_state, next_player(new_state.current_turn()));
	});
	return ret;
    }


    vector<Move> _available_moves(const ReferenceGameState & state) {
	vector<Move> moves = _available_moves_without_check(state);
	auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
		return _results_in_check(state, move);
	    });
	moves.erase(new_end, moves.end());
	return moves;
    }

    bool _is_legal_move(const ReferenceGameState& state, const Move& move, bool allow_check) {
	vector<Move> moves =
	    allow_check
	    ? _available_moves_without_check(state)
	    : _available_moves(state);
	return !(std::find(moves.begin(), moves.end(), move) == moves.end());
    }


    bool _is_king_in_check(const ReferenceGameState& original_state, Player player) {
	auto state = original_state;
	state.current_turn(next_player(player));
	auto king_position = state.get_king_position(player);
	if (!king_position)
	    return false;
    
	return !_captures_for_position(state, *king_position).empty();
    }

    Player _winner(const ReferenceGameState& state) {
	auto winner = __winner(state);
	if (!winner)
	    throw logic_error("Winner asked for when no winner");
	return *winner;
    }

    bool _is_winner(const ReferenceGameState& state) {
	return !!__winner(state);
    }

    bool _is_capture(const ReferenceGameState& state, const Move& move) {
	return !!state.get_piece(move.to);
    }

    vector<Move> _filter_available_moves(const ReferenceGameState& state, function<bool(const Move&)> pred)
    {
	auto moves = _available_moves_without_check(state);
	auto matches = vector<Move>();
	for_each(moves.begin(), moves.end(), [&] (const Move& move) {
		if (pred(move))
		    matches.push_back(move);
	    });
	return matches;
    }

    vector<Move> _captures_for_position(const ReferenceGameState& state, const Position& position)
    {
	return _filter_available_moves(state, [&] (const Move& move) {
	    return move.to == position;
        });
    }

    vector<Move> _available_moves_from(const ReferenceGameState& state, const Position& position)
    {
	return _filter_available_moves(state, [&] (const Move& move) {
	    return move.from == position;
	});
    }

    int _num_available_moves(const ReferenceGameState& state)
    {
	return _available_moves(state).size();
    }

    int _num_available_captures(const ReferenceGameState& state)
    {
	return _filter_available_moves(state, [&] (const Move& move) {
	    return _is_capture(state, move);
        }).size();
    }
};


