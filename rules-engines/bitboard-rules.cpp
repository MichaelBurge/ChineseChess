#include <algorithm>
#include <stdint.h>
#include "bitboard-gamestate.hpp"
#include "bitboard-rules.hpp"
#include <iostream>
using namespace std;

namespace bitboard_implementation {

uint8_t  minimal_pos(bitboard board, Direction direction) {
    return (direction == NORTH || direction == WEST)
	? lsb_first_set(board)
	: msb_first_set(board);
}

bool is_red_side(Position position) {
    return position.value < 45;
}

bitboard generate_entire_board() {
    bitboard ret(0, 0);
    for (uint8_t i = 0; i < 90; i++)
	ret.set(i);
    return ret;
}

bitboard generate_castle_area() {
    bitboard ret(0, 0);
    for (uint8_t i = 4; i <= 6; i++) {
	for (uint8_t j = 1; j <= 3; j++) {
	    ret.set(Position(j, i).value);
	}
	for (uint8_t j = 8; j <= 10; j++) {
	    ret.set(Position(j, i).value);
	}
    }
    return ret;
}

bitboard generate_red_side() {
    bitboard ret(0, 0);
    for (uint8_t i = 0; i < 45; i++) {
	ret.set(i);
    }
    return ret;
}

bitboard get_entire_board() {
    static bitboard reference = generate_entire_board();
    return reference;
}

bitboard get_castle_area() {
    static bitboard reference = generate_castle_area();
    return reference;
}

bitboard get_red_side() {
    static bitboard reference = generate_red_side();
    return reference;
}

const LookupTable& _red_soldier_moves_lookup_table() {
    static LookupTable reference = generate_soldier_moves_lookup_table(false);
    return reference;
}

const LookupTable& _black_soldier_moves_lookup_table() {
    static LookupTable reference = generate_soldier_moves_lookup_table(true);
    return reference;
}

const LookupTable& _general_moves_lookup_table() {
    static LookupTable reference = generate_general_moves_lookup_table();
    return reference;
}

const LookupTable& _advisor_moves_lookup_table() {
    static LookupTable reference = generate_advisor_moves_lookup_table();
    return reference;
}

const DirectionalLookupTable& _horse_moves_lookup_table() {
	static DirectionalLookupTable reference = generate_horse_moves_lookup_table();
	return reference;
}

const DirectionalLookupTable& _chariot_ideal_moves_table() {
	static DirectionalLookupTable reference = generate_chariot_ideal_moves_table();
	return reference;
}

LookupTable generate_soldier_moves_lookup_table(bool swap_board) {
    LookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
        auto forward = move_direction(Position(i), swap_board ? SOUTH : NORTH);
	if (forward.value < 90)
	    ret.boards[i].set(forward.value);
	if (!(is_red_side(i) ^ swap_board)) {
	    auto west = move_direction(Position(i), WEST);
	    auto east = move_direction(Position(i), EAST);
	    ret.boards[i].set(west.value);
	    ret.boards[i].set(east.value);
	}
    }
    return ret;
}

LookupTable generate_general_moves_lookup_table() {
    LookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
	ret.boards[i].set(move_direction(Position(i), NORTH).value);
	ret.boards[i].set(move_direction(Position(i), SOUTH).value);
	ret.boards[i].set(move_direction(Position(i), EAST).value);
	ret.boards[i].set(move_direction(Position(i), WEST).value);
	ret.boards[i] &= get_castle_area();
    }
    return ret;
}

LookupTable generate_advisor_moves_lookup_table() {
    LookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
	ret.boards[i].set(move_direction(Position(i), NORTHEAST).value);
	ret.boards[i].set(move_direction(Position(i), NORTHWEST).value);
	ret.boards[i].set(move_direction(Position(i), SOUTHEAST).value);
	ret.boards[i].set(move_direction(Position(i), SOUTHWEST).value);
	ret.boards[i] &= get_castle_area();
    }
    return ret;
}

DirectionalLookupTable generate_horse_moves_lookup_table() {
    DirectionalLookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
	auto position = Position(i);
	ret.tables[0].boards[i].set(
            move_direction(move_direction(position, NORTH), NORTHEAST).value);
	ret.tables[0].boards[i].set(
            move_direction(move_direction(position, NORTH), NORTHWEST).value);

	ret.tables[1].boards[i].set(
            move_direction(move_direction(position, WEST), NORTHWEST).value);
	ret.tables[1].boards[i].set(
            move_direction(move_direction(position, WEST), SOUTHWEST).value);

	ret.tables[2].boards[i].set(
            move_direction(move_direction(position, SOUTH), SOUTHEAST).value);
	ret.tables[2].boards[i].set(
            move_direction(move_direction(position, SOUTH), SOUTHWEST).value);

	ret.tables[3].boards[i].set(
            move_direction(move_direction(position, EAST), NORTHEAST).value);
	ret.tables[3].boards[i].set(
            move_direction(move_direction(position, EAST), SOUTHEAST).value);
    }
    return ret;
}

DirectionalLookupTable generate_chariot_ideal_moves_table() {
    DirectionalLookupTable ret;
    for (uint8_t direction = 0; direction < 4; direction++) {
	for (uint8_t position = 0; position < 90; position++) {
	    Position iter = Position(position);
	    while ((iter = move_direction(iter, (Direction)direction)).value < 90)
		ret.tables[direction].boards[position].set(iter.value);
	}
    }
    return ret;
}

bitboard moves_for_red_soldier(Position position) {
    return _red_soldier_moves_lookup_table().boards[position.value];
}

bitboard moves_for_black_soldier(Position position) {
    return _black_soldier_moves_lookup_table().boards[position.value];
}

bitboard moves_for_general(Position position) {
    return _general_moves_lookup_table().boards[position.value];
}

bitboard moves_for_advisor(Position position) {
    return _advisor_moves_lookup_table().boards[position.value];
}

bitboard moves_for_horse(const BitboardGameState& state, Position position) {
    // Blockers = 4 groups of 64 bitboards set to 1 if the position blocks a horse(in a direction)
    bitboard accum;
    if (!(state.all_pieces.get(move_direction(position, NORTH).value)))
	accum |= _horse_moves_lookup_table().tables[0].boards[position.value];
    if (!(state.all_pieces.get(move_direction(position, WEST).value)))
	accum |= _horse_moves_lookup_table().tables[1].boards[position.value];
    if (!(state.all_pieces.get(move_direction(position, SOUTH).value)))
	accum |= _horse_moves_lookup_table().tables[2].boards[position.value];
    if (!(state.all_pieces.get(move_direction(position, EAST).value)))
	accum |= _horse_moves_lookup_table().tables[3].boards[position.value];
    return accum;
}

bitboard moves_for_elephant(const BitboardGameState& state, Position position) {
    bitboard accum;
    if (!(state.all_pieces.get(move_direction(position, NORTHEAST).value)))
	accum.set(multi_move_direction(position, NORTHEAST).value);
    if (!(state.all_pieces.get(move_direction(position, SOUTHEAST).value)))
	accum.set(multi_move_direction(position, SOUTHEAST).value);
    if (!(state.all_pieces.get(move_direction(position, NORTHWEST).value)))
	accum.set(multi_move_direction(position, NORTHWEST).value);
    if (!(state.all_pieces.get(move_direction(position, SOUTHWEST).value)))
	accum.set(multi_move_direction(position, SOUTHWEST).value);
    return accum;
}

bitboard moves_for_chariot(const BitboardGameState& state, Position position) {
    bitboard accum;
    for (uint8_t direction = 0; direction < 4; direction++) {
	bitboard ideal = _chariot_ideal_moves_table().tables[direction].boards[position.value];
	bitboard blockers = ideal & state.all_pieces;
        uint8_t first_blocker = minimal_pos(blockers, (Direction)direction);
	bitboard moves =
	    ideal ^
	    (
	     (first_blocker < 90)
	     ? _chariot_ideal_moves_table().tables[direction].boards[first_blocker]
	     : bitboard(0, 0)
	    );
								    
	accum |= moves;
    }
    return accum;
}

bitboard moves_for_cannon(const BitboardGameState& state, Position position) {
    bitboard accum;
    for (uint8_t direction = 0; direction < 4; direction++) {
	bitboard ideal = _chariot_ideal_moves_table().tables[direction].boards[position.value];
	bitboard blockers = ideal & state.all_pieces;
	if (!blockers) {
	    accum |= ideal;
	} else {
	    uint8_t first_blocker = minimal_pos(blockers, (Direction)direction);
	    blockers.toggle(first_blocker);
	    uint8_t second_blocker = minimal_pos(blockers, (Direction)direction);
	    bitboard moves =
		ideal ^
		_chariot_ideal_moves_table().tables[direction].boards[first_blocker];
	    moves.toggle(first_blocker);
	    if (second_blocker < 90)
		moves.toggle(second_blocker);
	    accum |= moves;
	}
    }
    return accum;
}

bitboard moves_for_piece(const BitboardGameState& state, Position position, Piece piece) {
    bitboard ret;
    switch (piece) {
    case RED_SOLDIER:
	ret = moves_for_red_soldier(position); break;
    case BLACK_SOLDIER:
	ret = moves_for_black_soldier(position); break;
    case RED_GENERAL:
    case BLACK_GENERAL:
	ret = moves_for_general(position); break;
    case RED_ADVISOR:
    case BLACK_ADVISOR:
	ret = moves_for_advisor(position); break;
    case RED_HORSE:
    case BLACK_HORSE:
	ret = moves_for_horse(state, position); break;
    case RED_ELEPHANT:
    case BLACK_ELEPHANT:
	ret = moves_for_elephant(state, position); break;
    case RED_CHARIOT:
    case BLACK_CHARIOT:
	ret = moves_for_chariot(state, position); break;
    case RED_CANNON:
    case BLACK_CANNON:
	ret = moves_for_cannon(state, position); break;
    default:
	throw logic_error("Unknown piece"); break;
    }
    ret &= (state.current_turn() == RED)
	? ~(state.red_pieces)
	: ~(state.black_pieces);
    return ret;
}

void insert_vectorized_moves(const bitboard& board, const Position& root, vector<Move>& moves) {
    bitboard copy = board;
    uint8_t i = 0;
    while ((i = msb_first_set(copy)) < 90) {
	moves.push_back(Move(root, Position(i)));
	copy.clear(i);
    }
}

void ensure_moves_cached(const BitboardGameState& state) {
    if (state.is_cache_valid)
	return;
    state.moves = compute_reachable_positions(state);
    state.is_cache_valid = true;
}

bitboard compute_reachable_positions(const BitboardGameState& state) {
    bitboard reachable_positions;
    for (const Cell& cell : state.pieces) {
	if (owner(cell.piece) != state.current_turn())
	    continue;
	reachable_positions |= moves_for_piece(state, cell.position, cell.piece);
    }
    return reachable_positions;
}

vector<Move> _available_moves(const BitboardGameState& state) {
    // TODO: This could be really slow. See if caching the positions that result in check helps.
    vector<Move> moves = _available_moves_without_check(state);
    auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) -> bool {
        return _results_in_check(state, move);
    });
    moves.erase(new_end, moves.end());
    return moves;
}\

vector<Move> _available_moves_without_check(const BitboardGameState& state) {
    // TODO: See if assuming ~38 moves is a performance improvement
    auto moves = vector<Move>();
    for (const Cell& cell : state.pieces) {
	if (owner(cell.piece) != state.current_turn())
	    continue;
	bitboard piece_moves = moves_for_piece(state, cell.position, cell.piece);
	insert_vectorized_moves(piece_moves, cell.position, moves);
    }
    return moves;
}

bool _is_capture(const BitboardGameState& state, const Move& move) {
    return state.all_pieces.get(move.to.value);
}

bool _is_legal_move(const BitboardGameState& state, const Move& move, bool allow_check) {
    ensure_moves_cached(state);
    auto moves = allow_check
	? _available_moves_without_check(state)
	: _available_moves(state);
    for (const Move& candidate : moves) {
	if (move == candidate)
	    return true;
    }
    return false;
}

Player _winner(const BitboardGameState& state) {
    ensure_moves_cached(state);
    if (!state.moves)
	return next_player(state.current_turn());
    else
	throw logic_error("No moves available");
}

bool _is_winner(const BitboardGameState& state) {
    ensure_moves_cached(state);
    return !state.moves;
}

bool _is_king_in_check(const BitboardGameState& state, Player player) {
    if (player != state.current_turn()) {
	ensure_moves_cached(state);
	return !!(state.generals & state.moves);
    } else {
	auto& mutable_state = const_cast<BitboardGameState&>(state);
	mutable_state.switch_turn();
	auto reachable_positions = compute_reachable_positions(mutable_state);
	bool is_check = !!(reachable_positions & mutable_state.generals);
	mutable_state.switch_turn();
	return is_check;
    }
}

vector<Move> _filter_available_moves(const BitboardGameState& state, function<bool(const Move&)> pred) {
    throw logic_error("_filter_available_moves is unimplemented");
    auto moves = vector<Move>();
    for (const Move& move : _available_moves(state)) {
	if (pred(move))
	    moves.push_back(move);
    }
    return moves;
}

vector<Move> _captures_for_position(const BitboardGameState& state, const Position& position) {
    throw logic_error("_captures_for_position is Unimplemented");
}

vector<Move> _available_moves_from(const BitboardGameState& state, const Position& position) {
    auto moves = vector<Move>();
    auto piece = state.get_piece(position);
    auto piece_reachable = moves_for_piece(state, position, piece);
    insert_vectorized_moves(piece_reachable, position, moves);
    return moves;
}

int _num_available_moves(const BitboardGameState& state) {
    return _available_moves(state).size();
}

int _num_available_captures(const BitboardGameState& state) {
    auto moves = _available_moves(state);
    auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& move) {
        return !_is_capture(state, move);
    });
    moves.erase(new_end, moves.end());
    return moves.size();
}

bool _results_in_check(const BitboardGameState& state, const Move& move) {
    bool result;
    Player current_turn = state.current_turn();
    state.peek_move(move, [&] (const BitboardGameState& newState) {
	result = _is_king_in_check(newState, current_turn);
    });
    return result;
}

};
