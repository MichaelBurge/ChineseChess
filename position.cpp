#include "position.hpp"
#include <boost/lexical_cast.hpp>
using namespace boost;

bool Position::is_valid() const {
    return value < 90;
}

Position::Position(uint8_t rank, uint8_t file) :
    value(
	  (
	   rank >= 1 &&
	   rank <= 10 &&
	   file >= 1 &&
	   file <= 9
	  )
	  ? (rank-1) * 9 + (file-1)
	  : 255
	 ) { }

uint8_t Position::rank() const {
    return value / 9 + 1;
}

uint8_t Position::file() const {
    return value % 9 + 1;
}

bool Position::operator<(const Position& b) const {
    return value < b.value;
}

bool Position::operator==(const Position& b) const {
    return value == b.value;
}

char file_display(int file) {
    return 'A' + file - 1;
}

string position_repr(const Position& position) {
    return file_display(position.file()) + lexical_cast<string>((int)position.rank());
}

ostream& operator<<(ostream& os, const Position& position) {
    return os << position_repr(position);
}
