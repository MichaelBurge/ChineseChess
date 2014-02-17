#pragma once

#include "../position.hpp"
#include "../uint128_t.hpp"

typedef uint128_t bitboard;

extern bitboard flip(const bitboard& board);
extern void     print_bitboard(ostream&, const bitboard& board);
