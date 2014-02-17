#include "bitboard.hpp"

constexpr uint64_t first_26_bits = 0x3FFFFFF;

bitboard flip(const bitboard& board) {
    bitboard ret;
    ret.msb = __builtin_bswap64(board.lsb);
    ret.lsb =
	(__builtin_bswap64(board.msb) >> 26) |
	(board.msb & first_26_bits) << 26;
    ret.msb >>= 26;
    ret.msb &= first_26_bits;
    return ret;
}
