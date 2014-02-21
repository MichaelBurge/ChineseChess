#pragma once

#include <stdint.h>
#include <ostream>
using namespace std;

struct uint128_t {
    uint128_t() : msb(0), lsb(0) {}
    uint128_t(uint64_t msb, uint64_t lsb) : msb(msb), lsb(lsb) {}
    uint64_t msb;
    uint64_t lsb;
    inline bool get(uint8_t position) const
    {
	if (position < 64)
	    return (lsb & (1ULL << position)) != 0;
	else
	    return (msb & (1ULL << (position - 64))) != 0;
    }

    inline void set(uint8_t position)
    {
	if (position < 64)
	    lsb |= 1ULL << position;
	else
	    msb |= 1ULL << (position - 64);
    }

    inline void clear(uint8_t position)
    {
	if (position < 64)
	    lsb &= ~(1ULL << position);
	else
	    msb &= ~(1ULL << (position - 64));
    }

    inline void toggle(uint8_t position)
    {
	if (position < 64)
	    lsb ^= 1ULL << position;
	else
	    msb ^= 1ULL << (position - 64);
    }
    inline uint128_t& operator=(const uint128_t& b)
    { msb = b.msb; lsb = b.lsb; return *this; }

};


inline uint128_t& operator|=(uint128_t& a, const uint128_t& b)
{ a.msb |= b.msb; a.lsb |= b.lsb; return a; }

inline uint128_t& operator&=(uint128_t& a, const uint128_t& b)
{ a.msb &= b.msb; a.lsb &= b.lsb; return a; }

inline uint128_t& operator^=(uint128_t& a, const uint128_t& b)
{ a.msb ^= b.msb; a.lsb ^= b.lsb; return a; }

inline uint128_t operator| (const uint128_t& a, const uint128_t& b)
{ return uint128_t(a.msb | b.msb, a.lsb | b.lsb); }

inline uint128_t operator& (const uint128_t& a, const uint128_t& b)
{ return uint128_t(a.msb & b.msb, a.lsb & b.lsb); }

inline uint128_t operator^ (const uint128_t& a, const uint128_t& b)
{ return uint128_t(a.msb ^ b.msb, a.lsb ^ b.lsb); }

inline uint128_t operator~(const uint128_t& a)
{ return uint128_t(~a.msb, ~a.lsb); }

inline bool operator==(const uint128_t& a, const uint128_t& b)
{ return a.msb == b.msb && a.lsb == b.lsb; }

inline bool operator!(const uint128_t& x)
{ return !(x.lsb > 0 || x.msb > 0); }

extern ostream& operator<<(ostream& os, const uint128_t& bits);
inline uint8_t msb_first_set(const uint128_t& board) {
    if (board.msb) {
	return (127 - __builtin_clzll((uint64_t)board.msb));
    } else if (board.lsb) {
	return (63 - __builtin_clzll((uint64_t)board.lsb));
    } else {
	return 255;
    }
}

inline uint8_t lsb_first_set(const uint128_t& board) {
    if (board.lsb) {
	return __builtin_ctzll(board.lsb);
    } else if (board.msb) {
	return __builtin_ctzll((uint64_t)board.msb) + 64;
    } else {
	return 255;
    }
}

inline uint8_t num_set(const uint128_t& board)
{ return __builtin_popcountll(board.msb) + __builtin_popcountll(board.lsb); }
