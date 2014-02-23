#include "position.hpp"
#include <boost/lexical_cast.hpp>
using namespace boost;

char file_display(int file) {
    return 'A' + file - 1;
}

string position_repr(const Position& position) {
    return file_display(position.file()) + lexical_cast<string>((int)position.rank());
}

ostream& operator<<(ostream& os, const Position& position) {
    return os << position_repr(position);
}
