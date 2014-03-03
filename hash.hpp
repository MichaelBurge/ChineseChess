#pragma once

#include "piece.hpp"
#include "position.hpp"

typedef uint64_t Hash;

// First digits of Pi
const int magic_random_seed = (int)3141592653;

struct ZobristTable {
    Hash zobrists[16][90];
};

extern ZobristTable& precomputed_zobrist_numbers();
extern ZobristTable set_zobrist_numbers();
extern ZobristTable& precomputed_zobrist_numbers();
extern void print_debug_zobrist_hashes();
inline void toggle_hash(Hash& hash, const Position& position, const Piece& piece) {
    auto& zobrists = precomputed_zobrist_numbers();
    hash ^= zobrists.zobrists[static_cast<int>(piece)][position.value];
}
