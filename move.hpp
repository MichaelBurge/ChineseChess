#pragma once

#include "position.hpp"
#include "direction.hpp"
#include <vector>
using namespace std;

struct Move {
    Position from;
    Position to;
    bool operator<(const Move& b) const;
    bool operator==(const Move& b) const;
    Move(const Position& from, const Position& to);
    Move(const Position& from, Direction direction);
};
ostream& operator<<(ostream& os, const Move& move);

string move_repr(const Move& move_repr);

void print_moves(const vector<Move>& moves);
