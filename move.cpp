#include "move.hpp"

ostream& operator<<(ostream& os, const Move& move) {
    return os << move.from << move.to;
}

Move mkMove(const Position& from, const Position& to) {
  auto ret = Move();
  ret.from = from;
  ret.to = to;
  return ret;
}

Move mkMove(const Position& from, Direction direction) {
    return mkMove(from, move_direction(from, direction));
}
