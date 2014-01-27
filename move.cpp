#include "move.hpp"

string move_repr(const Move& move) {
    return position_repr(move.from) + position_repr(move.to);
}

ostream& operator<<(ostream& os, const Move& move) {
    return os << move_repr(move);
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


bool Move::operator<(const Move& b) const {
    return (from < b.from) ||
            (from == b.from &&
             to < b.to);
}

bool Move::operator==(const Move& b) const {
    return this->from == b.from && this->to == b.to;
}
