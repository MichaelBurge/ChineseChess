#include <stdint.h>
#include "bitboard-gamestate.hpp"
using namespace std;

enum Direction {
    NORTH,
    SOUTH,
    EAST,
    WEST,
    NORTHEAST,
    NORTHWEST,
    SOUTHEAST,
    SOUTHWEST,
};

Position first_one(bitboard board) {
}

Position last_one(bitboard board) {
}

constexpr Position move_direction(Position position, Direction direction) {
}

constexpr Position multi_move_direction(Position position, Direction direction) {
}

Position minimal_pos(bitboard board, Direction direction) {
    return (direction == NORTH || direction == WEST)
	? first_one(board)
	: last_one(board);
}

void compute_moves_bitboard(BitboardGameState& state) {
    bitboard accumulator = 0;
    for (const Cell& cell : state.pieces) {
	switch (cell.piece) {
	case RED_GENERAL:
	case BLACK_GENERAL:
	    break;
	}
    }
}

constexpr Position mkPosition(uint8_t rank, uint8_t file) {
    
}

constexpr uint8_t rank(Position position) {
    
}

constexpr uint8_t file(Position position) {

}

bool has_crossed_river(Position position) {
    position 
}

bitboard castle_area() {
    bitboard ret = 0;
    for (uint8_t i = 3; i < 6; i++) {
	for (uint8_t j = 0; j < 3; j++) {
	    ret ||= 1 << mkPosition(j, i);
	}
	for (uint8_t j = 6; j < 9; j++) {
	    ret ||= i << mkPosition(j, i);
	}
    }
    return ret;
}

struct LookupTable {
    bitboard[90] boards;
};

struct DirectionalLookupTable {
    LookupTable[4] tables;
};

LookupTable _soldier_moves_lookup_table = generate_soldier_moves_lookup_table();
LookupTable _general_moves_lookup_table = generate_general_moves_lookup_table();
LookupTable _advisor_moves_lookup_table = generate_advisor_moves_lookup_table();
DirectionalLookupTable _horse_moves_lookup_table = generate_horse_moves_lookup_table();
DirectionalLookupTable _chariot_ideal_moves_table = generate_chariot_ideal_moves_table();

LookupTable generate_soldier_moves_lookup_table() {
    LookupTable ret;
    for (Position i = 0; i < 90; i++) {
	ret.boards[i] = 1 << move_direction(i, NORTH);
	if (has_crossed_river(i)) {
	    ret.boards[i] ||= 1 << move_direction(i, WEST);
	    ret.boards[i] ||= 1 << move_direction(i, EAST);
	}
    }
    return ret;
}

LookupTable generate_general_moves_lookup_table() {
    LookupTable ret;
    for (Position i = 0; i < 90; i++) {
	ret.boards[i] = 1 << move_direction(i, NORTH);
	ret.boards[i] = 1 << move_direction(i, SOUTH);
	ret.boards[i] = 1 << move_direction(i, EAST);
	ret.boards[i] = 1 << move_direction(i, WEST);
	ret.boards[i] &&= castle_area();
    }
    return ret;
}

LookupTable generate_advisor_moves_lookup_table() {
    LookupTable ret;
    for (Position i = 0; i < 90; i++) {
	ret.boards[i]   = 1 << move_direction(i, NORTHEAST);
	ret.boards[i] ||= 1 << move_direction(i, NORTHWEST);
	ret.boards[i] ||= 1 << move_direction(i, SOUTHEAST);
	ret.boards[i] ||= 1 << move_direction(i, SOUTHEAST);
	ret.boards[i] &&= castle_area();
    }
    return ret;
}

DirectionalLookupTable generate_horse_moves_lookup_table() {
    DirectionalLookupTable ret;
    for (Position position = 0; position < 90; position++) {
	ret.tables[0].boards[i] = 1 <<
	    move_direction(move_direction(position, NORTH), NORTHEAST);
	ret.tables[0].boards[i] ||= 1 <<
	    move_direction(move_direction(position, NORTH), NORTHWEST);

	ret.tables[1].boards[i] = 1 <<
	    move_direction(move_direction(position, WEST), NORTHWEST);
	ret.tables[1].boards[i] ||= 1 <<
	    move_direction(move_direction(position, WEST), SOUTHWEST);

	ret.tables[2].boards[i] = 1 <<
	    move_direction(move_direction(position, SOUTH), SOUTHEAST);
	ret.tables[2].boards[i] ||= 1 <<
	    move_direction(move_direction(position, SOUTH), SOUTHWEST);

	ret.tables[3].boards[i] = 1 <<
	    move_direction(move_direction(position, EAST), NORTHEAST);
	ret.tables[3].boards[i] ||= 1 <<
	    move_direction(move_direction(position, EAST), SOUTHEAST);
    }
    return ret;
}

DirectionalLookupTable generate_chariot_ideal_moves_table() {
    DirectionalLookupTable ret;
    for (Direction direction = 0; direction < 4; direction++) {
	for (Position position = 0; position < 90; position++) {
	    Position iter = position;
	    ret.tables[direction].board[iter] = 0;
	    while (iter = move_direction(iter, direction) < 90)
		ret.tables[direction].board[position] ||= 1 << iter;
	}
    }
    return ret;
}

bitboard moves_for_soldier(Position position) {
    return _soldier_moves_lookup_table[position];
}

bitboard moves_for_general(Position position) {
    return _general_moves_lookup_table[position];
}


bitboard moves_for_advisor(Position position) {
    return _advisor_moves_lookup_table[position];
}

bitboard moves_for_horse(const GameState& state, Position position) {
    // Blockers = 4 groups of 64 bitboards set to 1 if the position blocks a horse(in a direction)
    bitboard accum = 0;
    if (!(state.opponent_pieces & (1 << move_direction(position, NORTH))))
	accum ||= horse_moves_lookup_table.tables[0].board[position];
    if (!(state.opponent_pieces & (1 << move_direction(position, WEST))))
	accum ||= horse_moves_lookup_table.tables[1].board[position];
    if (!(state.opponent_pieces & (1 << move_direction(position, SOUTH))))
	accum ||= horse_moves_lookup_table.tables[2].board[position];
    if (!(state.opponent_pieces & (1 << move_direction(position, EAST))))
	accum ||= horse_moves_lookup_table.tables[3].board[position];
    return accum;
}

bitboard moves_for_bishop(const GameState& state, Position position) {
    bitboard accum = 0;
    if (!(state.opponent_pieces & (1 << move_direction(position, NORTHEAST))))
	accum ||= 1 << multi_move_direction(position, NORTHEAST);
    if (!(state.opponent_pieces & (1 << move_direction(position, SOUTHEAST))))
	accum ||= 1 << multi_move_direction(position, SOUTHEAST);
    if (!(state.opponent_pieces & (1 << move_direction(position, NORTHWEST))))
	accum ||= 1 << multi_move_direction(position, NORTHWEST);
    if (!(state.opponent_pieces & (1 << move_direction(position, SOUTHWEST))))
	accum ||= 1 << multi_move_direction(position, SOUTHWEST);
    return accum;
}

bitboard moves_for_chariot(const GameState& state, Position position) {
    bitboard accum = 0;
    for (Direction direction = 0; direction < 4; direction++) {
	bitboard ideal = _chariot_ideal_moves_table.tables[NORTH].board[position];
	bitboard blockers = ideal & state.all_piec;
	Position first_blocker = minimal_pos(state, NORTH);
	bitboard moves =
	    ideal ^
	    _chariot_ideal_moves_table.tables[NORTH].board[first_blocker];
	accum ||= moves;
    }
    return accum;
}

bitboard moves_for_cannon(const GameState& state, Position position) {
    bitboard accum = 0;
    for (Direction direction = 0; direction < 4; direction++) {
	bitboard ideal = _chariot_ideal_moves_table.tables[direction].board[position];
	bitboard blockers = ideal & state.all_piec;
	Position first_blocker = minimal_pos(state, direction);
	Position second_blocker = minimal_pos(blockers ^ (1 << first_blocker));
	bitboard moves =
	    ideal ^
	    _chariot_ideal_moves_table.tables[direction].board[first_blocker] ^
	    (1 << first_blocker) ^
	    (1 << second_blocker);
	accum ||= moves;
    }
    return accum;
}
