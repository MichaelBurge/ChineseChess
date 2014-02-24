#include <algorithm>
#include <stdint.h>
#include "bitboard.hpp"
#include "bitboard-gamestate.hpp"
#include "bitboard-rules.hpp"
#include <iostream>
using namespace std;

namespace bitboard_implementation {
    static bitboard generate_castle_area();
    static bitboard generate_red_side();
    static LookupTable generate_soldier_moves_lookup_table(bool);
    static LookupTable generate_general_moves_lookup_table();
    static LookupTable generate_advisor_moves_lookup_table();
    static DirectionalLookupTable generate_horse_moves_lookup_table();
    static DirectionalLookupTable generate_chariot_ideal_moves_table();
    static bitboard moves_for_red_soldier(Position position);
    static bitboard moves_for_black_soldier(Position position);
    static bitboard moves_for_elephant(const BitboardGameState& state, Position position);
    static bitboard moves_for_general(Position position);
    static bitboard moves_for_advisor(Position position);
    static bitboard moves_for_horse(const BitboardGameState& state, Position position);
    static bitboard moves_for_chariot(const BitboardGameState& state, Position position);
    static bitboard moves_for_cannon(const BitboardGameState& state, Position position);
    static bitboard compute_red_reachable_positions(const BitboardGameState& state);
    static bitboard compute_black_reachable_positions(const BitboardGameState& state);
    static inline bitboard compute_reachable_positions(const BitboardGameState& state) {
	if (state.current_turn() == RED)
	    return compute_red_reachable_positions(state);
	else
	    return compute_black_reachable_positions(state);
    }
    static void ensure_moves_cached(const BitboardGameState& state);
    static bool _violates_flying_kings_rule(const BitboardGameState& state);
    static uint8_t  minimal_pos(bitboard board, Direction direction);

uint8_t  minimal_pos(bitboard board, Direction direction) {
    return (direction == NORTH || direction == WEST)
	? lsb_first_set(board)
	: msb_first_set(board);
}

bool is_red_side(Position position) {
    return position.value < 45;
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
        ret.boards[i] &= get_entire_board();
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
	auto northeast = move_direction(Position(i), NORTHEAST).value;
	auto northwest = move_direction(Position(i), NORTHWEST).value;
        auto southeast = move_direction(Position(i), SOUTHEAST).value;
	auto southwest = move_direction(Position(i), SOUTHWEST).value;

	if (northeast < 90)
	    ret.boards[i].set(northeast);
	if (northwest < 90)
	    ret.boards[i].set(northwest);
	if (southeast < 90)
	    ret.boards[i].set(southeast);
	if (southwest < 90)
	    ret.boards[i].set(southwest);

	ret.boards[i] &= get_castle_area();
    }
    return ret;
}

DirectionalLookupTable generate_horse_moves_lookup_table() {
    DirectionalLookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
	auto position = Position(i);
	ret.tables[NORTH].boards[i].set(
            move_direction(move_direction(position, NORTH), NORTHEAST).value);
	ret.tables[NORTH].boards[i].set(
            move_direction(move_direction(position, NORTH), NORTHWEST).value);
        ret.tables[NORTH].boards[i] &= get_entire_board();

	ret.tables[WEST].boards[i].set(
            move_direction(move_direction(position, WEST), NORTHWEST).value);
	ret.tables[WEST].boards[i].set(
            move_direction(move_direction(position, WEST), SOUTHWEST).value);
        ret.tables[WEST].boards[i] &= get_entire_board();

	ret.tables[SOUTH].boards[i].set(
            move_direction(move_direction(position, SOUTH), SOUTHEAST).value);
	ret.tables[SOUTH].boards[i].set(
            move_direction(move_direction(position, SOUTH), SOUTHWEST).value);
        ret.tables[SOUTH].boards[i] &= get_entire_board();

	ret.tables[EAST].boards[i].set(
            move_direction(move_direction(position, EAST), NORTHEAST).value);
	ret.tables[EAST].boards[i].set(
            move_direction(move_direction(position, EAST), SOUTHEAST).value);
        ret.tables[EAST].boards[i] &= get_entire_board();
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
    uint8_t north = move_direction(position, NORTH).value;
    uint8_t south = move_direction(position, SOUTH).value;
    uint8_t west = move_direction(position, WEST).value;
    uint8_t east = move_direction(position, EAST).value;

    bool is_north_blocked = (north >= 90) || state.all_pieces.get(north);
    bool is_south_blocked = (south >= 90) || state.all_pieces.get(south);
    bool is_west_blocked  = (west  >= 90) || state.all_pieces.get(west);
    bool is_east_blocked  = (east  >= 90) || state.all_pieces.get(east);

    if (!is_north_blocked)
	accum |= _horse_moves_lookup_table().tables[NORTH].boards[position.value];
    if (!is_south_blocked)
	accum |= _horse_moves_lookup_table().tables[SOUTH].boards[position.value];
    if (!is_west_blocked)
	accum |= _horse_moves_lookup_table().tables[WEST].boards[position.value];
    if (!is_east_blocked)
	accum |= _horse_moves_lookup_table().tables[EAST].boards[position.value];
    return accum;
}

bitboard moves_for_elephant(const BitboardGameState& state, Position position) {
    bitboard accum;

    uint8_t northeast = move_direction(position, NORTHEAST).value;
    uint8_t southeast = move_direction(position, SOUTHEAST).value;
    uint8_t northwest = move_direction(position, NORTHWEST).value;
    uint8_t southwest = move_direction(position, SOUTHWEST).value;

    bool is_northeast_blocked = (northeast >= 90) || state.all_pieces.get(northeast);
    bool is_southeast_blocked = (southeast >= 90) || state.all_pieces.get(southeast);
    bool is_northwest_blocked = (northwest >= 90) || state.all_pieces.get(northwest);
    bool is_southwest_blocked = (southwest >= 90) || state.all_pieces.get(southwest);

    if (!is_northeast_blocked) {
	northeast = move_direction(Position(northeast), NORTHEAST).value;
	if (northeast < 90)
	    accum.set(northeast);
    }
    if (!is_southeast_blocked) {
	southeast = move_direction(Position(southeast), SOUTHEAST).value;
	if (southeast < 90)
	    accum.set(southeast);
    }
    if (!is_northwest_blocked) {
	northwest = move_direction(Position(northwest), NORTHWEST).value;
	if (northwest < 90)
	    accum.set(northwest);
    }
    if (!is_southwest_blocked) {
	southwest = move_direction(Position(southwest), SOUTHWEST).value;
	if (southwest < 90)
	    accum.set(southwest);
    }
    if (state.current_turn() == RED)
    	accum &= get_red_side();
    else
    	accum &= ~get_red_side();
    return accum;
}

bitboard moves_for_chariot(const BitboardGameState& state, Position position) {
    bitboard accum;

     bitboard scratch0 = _chariot_ideal_moves_table().tables[0].boards[position.value];
     bitboard scratch1 = _chariot_ideal_moves_table().tables[1].boards[position.value];
     bitboard scratch2 = _chariot_ideal_moves_table().tables[2].boards[position.value];
     bitboard scratch3 = _chariot_ideal_moves_table().tables[3].boards[position.value];

     accum |= scratch0;
     accum |= scratch1;
     accum |= scratch2;
     accum |= scratch3;

     scratch0 &= state.all_pieces;
     scratch1 &= state.all_pieces;
     scratch2 &= state.all_pieces;
     scratch3 &= state.all_pieces;

     uint8_t first_blocker0 = minimal_pos(scratch0, (Direction)0);
     uint8_t first_blocker1 = minimal_pos(scratch1, (Direction)1);
     uint8_t first_blocker2 = minimal_pos(scratch2, (Direction)2);
     uint8_t first_blocker3 = minimal_pos(scratch3, (Direction)3);

     if (first_blocker0 < 90)
	 accum ^= _chariot_ideal_moves_table().tables[0].boards[first_blocker0];
     if (first_blocker1 < 90)
	 accum ^= _chariot_ideal_moves_table().tables[1].boards[first_blocker1];
     if (first_blocker2 < 90)
	 accum ^= _chariot_ideal_moves_table().tables[2].boards[first_blocker2];
     if (first_blocker3 < 90)
	 accum ^= _chariot_ideal_moves_table().tables[3].boards[first_blocker3];
    return accum;
}

bitboard moves_for_cannon(const BitboardGameState& state, Position position) {
    bitboard accum;

    bitboard scratch0 = _chariot_ideal_moves_table().tables[0].boards[position.value];
    bitboard scratch1 = _chariot_ideal_moves_table().tables[1].boards[position.value];
    bitboard scratch2 = _chariot_ideal_moves_table().tables[2].boards[position.value];
    bitboard scratch3 = _chariot_ideal_moves_table().tables[3].boards[position.value];

    accum |= scratch0;
    accum |= scratch1;
    accum |= scratch2;
    accum |= scratch3;

    scratch0 &= state.all_pieces;
    scratch1 &= state.all_pieces;
    scratch2 &= state.all_pieces;
    scratch3 &= state.all_pieces;

    uint8_t first_blocker0 = minimal_pos(scratch0, (Direction)0);
    uint8_t first_blocker1 = minimal_pos(scratch1, (Direction)1);
    uint8_t first_blocker2 = minimal_pos(scratch2, (Direction)2);
    uint8_t first_blocker3 = minimal_pos(scratch3, (Direction)3);

    if (first_blocker0 < 90) {
	bitboard past_the_blocker = _chariot_ideal_moves_table().tables[0].boards[first_blocker0];
        accum ^= past_the_blocker;

	accum.clear(first_blocker0);
	scratch0.clear(first_blocker0);
	uint8_t second_blocker = minimal_pos(scratch0, (Direction)0);
	
	if (second_blocker < 90)
	    accum.set(second_blocker);
    }
    if (first_blocker1 < 90) {
	bitboard past_the_blocker = _chariot_ideal_moves_table().tables[1].boards[first_blocker1];
        accum ^= past_the_blocker;

	accum.clear(first_blocker1);
	scratch1.clear(first_blocker1);
	uint8_t second_blocker = minimal_pos(scratch1, (Direction)1);
	
	if (second_blocker < 90)
	    accum.set(second_blocker);
    }
    if (first_blocker2 < 90) {
	bitboard past_the_blocker = _chariot_ideal_moves_table().tables[2].boards[first_blocker2];
        accum ^= past_the_blocker;

	accum.clear(first_blocker2);
	scratch2.clear(first_blocker2);
	uint8_t second_blocker = minimal_pos(scratch2, (Direction)2);
	
	if (second_blocker < 90)
	    accum.set(second_blocker);
    }
    if (first_blocker3 < 90) {
	bitboard past_the_blocker = _chariot_ideal_moves_table().tables[3].boards[first_blocker3];
        accum ^= past_the_blocker;

	accum.clear(first_blocker3);
	scratch3.clear(first_blocker3);
	uint8_t second_blocker = minimal_pos(scratch3, (Direction)3);
	
	if (second_blocker < 90)
	    accum.set(second_blocker);
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
    case EMPTY:
	__builtin_unreachable();
    }
    ret &= (state.current_turn() == RED)
	? ~(state.red_pieces)
	: ~(state.black_pieces);
    return ret;
}

bool _violates_flying_kings_rule(const BitboardGameState& state) {
    // This is simalar to the chariot rule, but specialized for only one direction
    uint8_t first_king = minimal_pos(state.generals, SOUTH);
    bitboard king_killing_ray = _chariot_ideal_moves_table().tables[SOUTH].boards[first_king];
    uint8_t target = minimal_pos(king_killing_ray & state.all_pieces, SOUTH);
    return state.generals.get(target);
}

bool violates_flying_kings_rule(const BitboardGameState& state) {
    return _violates_flying_kings_rule(state);
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

bitboard compute_red_reachable_positions(const BitboardGameState& state) {
    uint8_t position;
    const bitboard& red_pieces = state.red_pieces;
    bitboard reachable_positions;
    bitboard candidates;

#define ACTION(position, piece) ({ reachable_positions |= moves_for_piece(state, position, piece); })

    ITER_PIECES(state.chariots & red_pieces, RED_CHARIOT, ACTION);
    ITER_PIECES(state.cannons & red_pieces, RED_CANNON, ACTION);
    ITER_PIECES(state.horses & red_pieces, RED_HORSE, ACTION);
    ITER_PIECES(state.soldiers & red_pieces, RED_SOLDIER, ACTION);
    ITER_PIECES(state.advisors & red_pieces, RED_ADVISOR, ACTION);
    ITER_PIECES(state.generals & red_pieces, RED_GENERAL, ACTION);
    ITER_PIECES(state.elephants & red_pieces, RED_ELEPHANT, ACTION);

#undef ACTION

    return reachable_positions;
}

bitboard compute_black_reachable_positions(const BitboardGameState& state) {
    uint8_t position;
    const bitboard& black_pieces = state.black_pieces;
    bitboard reachable_positions;
    bitboard candidates;

#define ACTION(position, piece) ({ reachable_positions |= moves_for_piece(state, position, piece); })

    ITER_PIECES(state.chariots & black_pieces, BLACK_CHARIOT, ACTION);
    ITER_PIECES(state.cannons & black_pieces, BLACK_CANNON, ACTION);
    ITER_PIECES(state.horses & black_pieces, BLACK_HORSE, ACTION);
    ITER_PIECES(state.soldiers & black_pieces, BLACK_SOLDIER, ACTION);
    ITER_PIECES(state.advisors & black_pieces, BLACK_ADVISOR, ACTION);
    ITER_PIECES(state.generals & black_pieces, BLACK_GENERAL, ACTION);
    ITER_PIECES(state.elephants & black_pieces, BLACK_ELEPHANT, ACTION);

#undef ACTION
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
    vector<Move> moves;
    moves.reserve(38);

    bitboard candidates;
    uint8_t position;

#define ACTION(position, piece) ({ bitboard piece_moves = moves_for_piece(state, position, piece); insert_vectorized_moves(piece_moves, position, moves); })

    if (state.current_turn() == RED) {
	ITER_PIECES(state.chariots & state.red_pieces, RED_CHARIOT, ACTION);
	ITER_PIECES(state.cannons & state.red_pieces, RED_CANNON, ACTION);
	ITER_PIECES(state.horses & state.red_pieces, RED_HORSE, ACTION);
	ITER_PIECES(state.soldiers & state.red_pieces, RED_SOLDIER, ACTION);
	ITER_PIECES(state.advisors & state.red_pieces, RED_ADVISOR, ACTION);
	ITER_PIECES(state.generals & state.red_pieces, RED_GENERAL, ACTION);
	ITER_PIECES(state.elephants & state.red_pieces, RED_ELEPHANT, ACTION);
    } else {
	ITER_PIECES(state.chariots & state.black_pieces, BLACK_CHARIOT, ACTION);
	ITER_PIECES(state.cannons & state.black_pieces, BLACK_CANNON, ACTION);
	ITER_PIECES(state.horses & state.black_pieces, BLACK_HORSE, ACTION);
	ITER_PIECES(state.soldiers & state.black_pieces, BLACK_SOLDIER, ACTION);
	ITER_PIECES(state.advisors & state.black_pieces, BLACK_ADVISOR, ACTION);
	ITER_PIECES(state.generals & state.black_pieces, BLACK_GENERAL, ACTION);
	ITER_PIECES(state.elephants & state.black_pieces, BLACK_ELEPHANT, ACTION);
    }
#undef ACTION

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
    return next_player(state.current_turn());
}

bool _is_winner(const BitboardGameState& state) {
    return _num_available_moves(state) == 0;
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
    auto moves = vector<Move>();
    for (const Move& move : _available_moves(state)) {
	if (pred(move))
	    moves.push_back(move);
    }
    return moves;
}

vector<Move> _captures_for_position(const BitboardGameState& state, const Position& position) {
    abort();
}

vector<Move> _available_moves_from(const BitboardGameState& state, const Position& position) {
    auto moves = _available_moves(state);
    auto new_end = remove_if(moves.begin(), moves.end(), [&] (const Move& candidate) {
	return !(candidate.from == position);
    });
    moves.erase(new_end, moves.end());
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
	result =
	    _is_king_in_check(newState, current_turn) ||
	    violates_flying_kings_rule(newState);
    });
    return result;
}

};
