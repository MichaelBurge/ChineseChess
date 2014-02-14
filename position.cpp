#include "position.hpp"
#include <boost/lexical_cast.hpp>
using namespace boost;

bool Position::is_valid() const {
  return
    1 <= file && file <= 9 &&
    1 <= rank && rank <= 10;
}

Position::Position(uint8_t rank, uint8_t file) : rank(rank), file(file), comparator((rank << 8) | file) { }

bool Position::operator<(const Position& b) const {
    return comparator < b.comparator;
}

bool Position::operator==(const Position& b) const {
    return comparator == b.comparator;
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
