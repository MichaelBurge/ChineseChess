#include "bitboard.hpp"
#include <ostream>
using namespace std;

constexpr uint64_t first_26_bits = 0x3FFFFFF;

void print_bitboard(ostream& os, const bitboard& board) {
    for (uint8_t rank = 1; rank != 11; rank++) {
	for (uint8_t file = 1; file != 10; file++) {
	    os << (board.get(Position(rank, file).value) ? '1' : '0');
	}
	os << endl;
    }
}
