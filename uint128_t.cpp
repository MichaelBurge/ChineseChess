#include "uint128_t.hpp"
#include <iomanip>
using namespace std;

ostream& operator<<(ostream& os, const uint128_t& bits) {
    auto print = [&] (uint64_t v, uint8_t o) {
	os << setw(2) << hex << ((v >> o) & 0xFF);
    };
    os << "0x" << hex << setw(2) << setfill('0');
    print(bits.msb, 56);
    print(bits.msb, 48);
    print(bits.msb, 40);
    print(bits.msb, 32);
    print(bits.msb, 24);
    print(bits.msb, 16);
    print(bits.msb, 8);
    print(bits.msb, 0);

    print(bits.lsb, 56);
    print(bits.lsb, 48);
    print(bits.lsb, 40);
    print(bits.lsb, 32);
    print(bits.lsb, 24);
    print(bits.lsb, 16);
    print(bits.lsb, 8);
    print(bits.lsb, 0);

    return os;
}

#include <iostream>
using namespace std;

uint8_t msb_first_set(const uint128_t& board) {
    if (board.msb) {
	return (127 - __builtin_clzll((uint64_t)board.msb));
    } else if (board.lsb) {
	return (63 - __builtin_clzll((uint64_t)board.lsb));
    } else {
	return 255;
    }
}

uint8_t lsb_first_set(const uint128_t& board) {
    if (board.lsb) {
	return __builtin_ctzll(board.lsb);
    } else if (board.msb) {
	return __builtin_ctzll((uint64_t)board.msb) + 64 - 1;
    } else {
	return 255;
    }
}
