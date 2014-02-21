#include "bitboard.hpp"
#include <ostream>
#include <iostream>
using namespace std;

constexpr uint64_t first_26_bits = 0x3FFFFFF;

bitboard generate_entire_board() {
    bitboard ret(0, 0);
    for (uint8_t i = 0; i < 90; i++)
	ret.set(i);
    return ret;
}

bitboard get_entire_board() {
    static bitboard reference = generate_entire_board();
    return reference;
}

void print_bitboard(const bitboard& board) {
    print_bitboard(cout, board);
}

bool has_extra_gunk(const bitboard& board) {
    return !!(board & ~(get_entire_board()));
}

void print_bitboard(ostream& os, const bitboard& board) {
    for (uint8_t rank = 1; rank != 11; rank++) {
	for (uint8_t file = 1; file != 10; file++) {
	    os << (board.get(Position(rank, file).value) ? '1' : '0');
	}
	os << endl;
    }
    if (has_extra_gunk(board)) {
	os << "And this board has some gunk in the hidden area..." << endl;
    }
}

