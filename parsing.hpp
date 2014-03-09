#include <boost/optional/optional.hpp>
#include "position.hpp"
#include "move.hpp"
using namespace boost;

template<typename T> optional<pair<T, string> > fail_parse() {
    return optional<pair<T, string> >();
}

template<typename T> optional<pair<T, string> > parse_value(const string&);
optional<pair<string, string> > parse_until(const string& text, function<bool(char)> pred);
optional<pair<string, string> > parse_token(const string& text, char delimiter);
