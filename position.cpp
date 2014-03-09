#include "parsing.hpp"
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

optional<pair<int, string> > parse_rank(const string& text) {
    auto a = parse_value<int>(text);
    if (!a)
        return fail_parse<int>();
    auto _pair = *a;
    auto value = _pair.first;
    if (1 <= value && value <= 10)
        return a;
    return fail_parse<int>();
}

optional<pair<int, string> > parse_file(const string& text) {
    auto c = parse_value<char>(text);
    if (!c)
        return fail_parse<int>();
    auto _pair = *c;
    auto value = toupper(_pair.first);
    if ('A' <= value && value <= 'I')
        return pair<int, string>(static_cast<int>(value - 'A' + 1), _pair.second);
    return fail_parse<int>();
}

template<> optional<pair<Position, string> > parse_value<Position>(const string& text) {
    auto f1 = parse_file(text);
    if (!f1)
        return fail_parse<Position>();
    auto r1 = parse_rank((*f1).second);
    if (!r1)
        return fail_parse<Position>();
    return pair<Position, string>(Position((*r1).first, (*f1).first), (*r1).second);
}
