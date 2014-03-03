#include "hash.hpp"
#include <iostream>
#include <random>
#include "configuration.hpp"
using namespace std;

ZobristTable set_zobrist_numbers() {
    ZobristTable ret;
    srand(magic_random_seed);
    mt19937_64 gen(magic_random_seed);
    Hash min = numeric_limits<Hash>::min();
    Hash max = numeric_limits<Hash>::max();
    uniform_int_distribution<Hash> dis(min, max);

    iter_all_pieces([&] (Piece piece) {
        for (int i = 0; i < 90; i++)
	    ret.zobrists[static_cast<Hash>(piece)][i] = dis(gen);
    });
    return ret;
}

ZobristTable& precomputed_zobrist_numbers() {
    static ZobristTable zobrists = set_zobrist_numbers();
    return zobrists;
}

void print_debug_zobrist_hashes() {
    auto& zobrists = precomputed_zobrist_numbers();
    iter_all_pieces([&] (Piece piece) {
	for (int x = 0; x < 90; x++) {
	    cout << "Position: "
		 << Position(x)
		 << ", Piece: "
		 << piece
		 << " - "
		 << zobrists.zobrists[static_cast<Hash>(piece)][x]
		 << endl;
	}
    });
}
