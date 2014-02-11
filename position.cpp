#include "position.hpp"
#include <boost/lexical_cast.hpp>
using namespace boost;

bool Position::is_valid() const {
  return
    1 <= file && file <= 9 &&
    1 <= rank && rank <= 10;
}

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
    return file_display(position.file) + lexical_cast<string>(position.rank);
}

ostream& operator<<(ostream& os, const Position& position) {
    return os << position_repr(position);
}
