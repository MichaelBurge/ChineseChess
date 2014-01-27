#include "position.hpp"

Position mkPosition(int rank, int file) {
  auto position = Position();
  position.rank = rank;
  position.file = file;
  return position;
}

bool Position::operator<(const Position& b) const {
  return (rank < b.rank) ||
          (rank == b.rank && file < b.file);
}

bool Position::operator==(const Position& b) const {
  return (rank == b.rank) && (file == b.file);
}

char file_display(int file) {
    return 'A' + file - 1;
}

string position_repr(const Position& position) {
    return file_display(position.file) + to_string(position.rank);
}

ostream& operator<<(ostream& os, const Position& position) {
    return os << position_repr(position);
}
