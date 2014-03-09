#pragma once

#include <boost/optional/optional.hpp>
#include <ostream>
#include <stdint.h>
using namespace std;
using namespace boost;

struct Position {
    inline Position() : value(numeric_limits<uint8_t>::max()) { }
    inline Position(uint8_t value) : value(value) { }
    inline Position(uint8_t rank, uint8_t file) :
	value(
	      (
	       rank >= 1 &&
	       rank <= 10 &&
	       file >= 1 &&
	       file <= 9
	       )
	      ? (rank-1) * 9 + (file-1)
	      : 255
	      ) { }

    inline uint8_t rank() const {
	return value / 9 + 1;
    }

    inline uint8_t file() const {
	return value % 9 + 1;
    }
    inline bool operator<(const Position& b) const
    { return value < b.value; 
}
    inline bool operator==(const Position& b) const
    { return value == b.value; }

    inline bool is_valid() const
    { return value < 90; }

    uint8_t value;
};

ostream& operator<<(ostream& os, const Position&);
string position_repr(const Position&);

extern optional<pair<int, string> > parse_rank(const string& text);
extern optional<pair<int, string> > parse_file(const string& text);
