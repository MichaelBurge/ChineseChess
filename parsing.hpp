#include <boost/optional/optional.hpp>
#include "rules.hpp"
using namespace boost;

template<typename T> optional<pair<T, string> > parse_value(const string&);
optional<pair<int, string> > parse_rank(const string& text);
optional<pair<int, string> > parse_file(const string& text);
optional<pair<Position, string> > parse_position(const string& text);
optional<pair<Move, string> > parse_move(const string& text);
