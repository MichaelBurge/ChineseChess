#include <stdint.h>
#include "bitboard-gamestate.hpp"
#include "bitboard-rules.hpp"
using namespace std;

namespace bitboard_implementation {

uint8_t  minimal_pos(bitboard board, Direction direction) {
    return (direction == NORTH || direction == WEST)
	? msb_first_set(board)
	: lsb_first_set(board);
}

bool is_red_side(Position position) {
    return position.value < 45;
}

bitboard castle_area() {
    bitboard ret(0, 0);
    for (uint8_t i = 3; i < 6; i++) {
	for (uint8_t j = 0; j < 3; j++) {
	    ret.set(Position(j, i).value);
	}
	for (uint8_t j = 6; j < 9; j++) {
	    ret.set(Position(j, i).value);
	}
    }
    return ret;
}

LookupTable _red_soldier_moves_lookup_table = generate_soldier_moves_lookup_table(false);
LookupTable _black_soldier_moves_lookup_table = generate_soldier_moves_lookup_table(true);
LookupTable _general_moves_lookup_table = generate_general_moves_lookup_table();
LookupTable _advisor_moves_lookup_table = generate_advisor_moves_lookup_table();
DirectionalLookupTable _horse_moves_lookup_table = generate_horse_moves_lookup_table();
DirectionalLookupTable _chariot_ideal_moves_table = generate_chariot_ideal_moves_table();

LookupTable generate_soldier_moves_lookup_table(bool swap_board) {
    LookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
	ret.boards[i].set(move_direction(Position(i), swap_board ? SOUTH : NORTH).value);
	if (is_red_side(i) ^ swap_board) {
	    ret.boards[i].set(1 << move_direction(Position(i), WEST).value);
	    ret.boards[i].set(1 << move_direction(Position(i), EAST).value);
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
	ret.boards[i] &= castle_area();
    }
    return ret;
}

LookupTable generate_advisor_moves_lookup_table() {
    LookupTable ret;
    for (uint8_t i = 0; i < 90; i++) {
	ret.boards[i].set(move_direction(Position(i), NORTHEAST).value);
	ret.boards[i].set(move_direction(Position(i), NORTHWEST).value);
	ret.boards[i].set(move_direction(Position(i), SOUTHEAST).value);
	ret.boards[i].set(move_direction(Position(i), SOUTHEAST).value);
	ret.boards[i] &= castle_area();
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

bitboard moves_for_soldier(const BitboardGameState& state, Position position) {
    return (state.current_turn() == RED)
	? _red_soldier_moves_lookup_table.boards[position.value]
	: _black_soldier_moves_lookup_table.boards[position.value];
}

bitboard moves_for_general(Position position) {
    return _general_moves_lookup_table.boards[position.value];
}

bitboard moves_for_advisor(Position position) {
    return _advisor_moves_lookup_table.boards[position.value];
}

bitboard moves_for_horse(const BitboardGameState& state, Position position) {
    // Blockers = 4 groups of 64 bitboards set to 1 if the position blocks a horse(in a direction)
    bitboard accum;
    if (!(state.all_pieces.get(move_direction(position, NORTH).value)))
	accum |= _horse_moves_lookup_table.tables[0].boards[position.value];
    if (!(state.all_pieces.get(move_direction(position, WEST).value)))
	accum |= _horse_moves_lookup_table.tables[1].boards[position.value];
    if (!(state.all_pieces.get(move_direction(position, SOUTH).value)))
	accum |= _horse_moves_lookup_table.tables[2].boards[position.value];
    if (!(state.all_pieces.get(move_direction(position, EAST).value)))
	accum |= _horse_moves_lookup_table.tables[3].boards[position.value];
    return accum;
}

bitboard moves_for_bishop(const BitboardGameState& state, Position position) {
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
	bitboard ideal = _chariot_ideal_moves_table.tables[direction].boards[position.value];
	bitboard blockers = ideal & state.all_pieces;
        uint8_t first_blocker = minimal_pos(blockers, (Direction)direction);
	bitboard moves =
	    ideal ^
	    _chariot_ideal_moves_table.tables[direction].boards[first_blocker];
	accum |= moves;
    }
    return accum;
}

bitboard moves_for_cannon(const BitboardGameState& state, Position position) {
    bitboard accum;
    for (uint8_t direction = 0; direction < 4; direction++) {
	bitboard ideal = _chariot_ideal_moves_table.tables[direction].boards[position.value];
	bitboard blockers = ideal & state.all_pieces;
	uint8_t first_blocker = minimal_pos(blockers, (Direction)direction);
	blockers.toggle(first_blocker);
	uint8_t second_blocker = minimal_pos(blockers, (Direction)direction);
	bitboard moves =
	    ideal ^
	    _chariot_ideal_moves_table.tables[direction].boards[first_blocker];
	moves.toggle(first_blocker);
	moves.toggle(second_blocker);
	accum |= moves;
    }
    return accum;
}

bitboard moves_for_piece(const BitboardGameState& state, Position position, Piece piece) {
    throw runtime_error("Unimplemented");
}

vector<Move> _available_moves(const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
    //for (const Cell& : state.pieces) {

    //}
}
vector<Move> _available_moves_without_check(const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

bool         _is_capture(const BitboardGameState&, const Move&) {
    throw runtime_error("Unimplemented");
}

bool         _is_legal_move(const BitboardGameState &, const Move&, bool allow_check) {
    throw runtime_error("Unimplemented");
}

Player       _winner(const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

bool         _is_winner(const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

bool         _is_king_in_check(const BitboardGameState& state, Player) {
    throw runtime_error("Unimplemented");
}

vector<Move> _filter_available_moves(const BitboardGameState& state, function<bool(const Move&)> pred) {
    throw runtime_error("Unimplemented");
}

vector<Move> _captures_for_position(const BitboardGameState& state, const Position& position) {
    throw runtime_error("Unimplemented");
}

vector<Move> _available_moves_from(const BitboardGameState& state, const Position& position) {
    throw runtime_error("Unimplemented");
}

int          _num_available_moves(const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

int          _num_available_captures(const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

bool         _results_in_check(const BitboardGameState& state, const Move& move) {
    throw runtime_error("Unimplemented");
}

};
