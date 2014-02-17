#pragma once

#include <stdint.h>
#include <ostream>
using namespace std;

struct uint128_t {
    uint128_t() : msb(0), lsb(0) {}
    uint128_t(uint64_t msb, uint64_t lsb) : msb(msb), lsb(lsb) {}
    uint64_t msb;
    uint64_t lsb;
    bool get(uint8_t position) const
    {
	if (position < 64)
	    return (lsb & (1ULL << position)) != 0;
	else
	    return (msb & (1ULL << (position - 64))) != 0;
    }

    void set(uint8_t position)
    {
	if (position < 64)
	    lsb |= 1ULL << position;
	else
	    msb |= 1ULL << (position - 64);
    }

    void clear(uint8_t position)
    {
	if (position < 64)
	    lsb &= 1ULL << position;
	else
	    msb &= 1ULL << (position - 64);
    }

    void toggle(uint8_t position)
    {
	if (position < 64)
	    lsb ^= 1ULL << position;
	else
	    msb ^= 1ULL << (position - 64);
    }

    bool operator==(uint128_t b) const
    { return msb == b.msb && lsb == b.lsb; }

    uint128_t operator&(uint128_t b) const
    { return uint128_t(msb & b.msb, lsb & b.lsb); }

    uint128_t operator|(uint128_t b) const
    { return uint128_t(msb | b.msb, lsb | b.lsb); }

    uint128_t operator^(uint128_t b) const
    { return uint128_t(msb ^ b.msb, lsb ^ b.lsb); }

    const uint128_t& operator&=(uint128_t b)
    { msb &= b.msb; lsb &= b.lsb; return *this; }

    const uint128_t& operator|=(uint128_t b)
    { msb |= b.msb; lsb |= b.lsb; return *this; }

    const uint128_t& operator^=(uint128_t b)
    { msb ^= b.msb; lsb ^= b.lsb; return *this; }
    bool operator!() { return !msb || !lsb; }
};

extern ostream& operator<<(ostream& os, const uint128_t& bits);
extern uint8_t msb_first_set(const uint128_t& value);
extern uint8_t lsb_first_set(const uint128_t& value);
extern uint8_t num_set(const uint128_t& value);
