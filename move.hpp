#pragma once

#include "position.hpp"
#include "direction.hpp"

struct Move {
  Position from;
  Position to;
  bool operator<(const Move& a) const;
};
ostream& operator<<(ostream& os, const Move& move);

Move mkMove(const Position& from, const Position& to);
Move mkMove(const Position& from, Direction direction);
