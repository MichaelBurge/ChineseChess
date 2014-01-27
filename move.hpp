#pragma once

#include "position.hpp"
#include "direction.hpp"

struct Move {
    Position from;
    Position to;
    bool operator<(const Move& b) const;
    bool operator==(const Move& b) const;
};
ostream& operator<<(ostream& os, const Move& move);

string move_repr(const Move& move_repr);

Move mkMove(const Position& from, const Position& to);
Move mkMove(const Position& from, Direction direction);
