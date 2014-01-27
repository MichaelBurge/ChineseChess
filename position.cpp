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

ostream& operator<<(ostream& os, const Position& position) {
    return os << file_display(position.file) << position.rank;
}
