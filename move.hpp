#pragma once

#include "position.hpp"
#include "direction.hpp"
#include <vector>
using namespace std;

struct Move {
    Position from;
    Position to;
    inline bool operator<(const Move& b) const {
	return (from < b.from) ||
            (from == b.from &&
             to < b.to);
    }

    inline bool operator==(const Move& b) const {
	return this->from == b.from && this->to == b.to;
    }

    inline Move(const Position& from, const Position& to) : from(from), to(to) { }
    inline Move(const Position& from, Direction direction) : from(from), to(move_direction(from, direction)) { }
};
ostream& operator<<(ostream& os, const Move& move);

string move_repr(const Move& move_repr);

void print_moves(const vector<Move>& moves);
