#include "parsing.hpp"
#include "position.hpp"
#include <boost/lexical_cast.hpp>
using namespace std;

template<> optional<pair<char, string> > parse_value(const string& text) {
    if (text.length() == 0)
        return optional<pair<char, string> >();
    auto ret = text[0];
    auto remaining = text.substr(1, text.length());
    return pair<char, string>(ret, remaining);
}

template<typename T> optional<pair<T, string> > fail_parse() {
    return optional<pair<T, string> >();
}

template<> optional<pair<int, string> > parse_value(const string& text) {
    auto parsed = parse_until(text, [] (char c) {
        return c < '0' || '9' < c;
    });
    if (!parsed)
        return fail_parse<int>();
    auto numerical_text = (*parsed).first;
    auto remaining_text = (*parsed).second;

    return pair<int, string>(lexical_cast<int>(numerical_text), remaining_text);
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

optional<pair<Position, string> > parse_position(const string& text) {
    auto f1 = parse_file(text);
    if (!f1)
        return fail_parse<Position>();
    auto r1 = parse_rank((*f1).second);
    if (!r1)
        return fail_parse<Position>();
    return pair<Position, string>(mkPosition((*r1).first, (*f1).first), (*r1).second);
}

optional<pair<Move, string> > parse_move(const string& text) {
    auto p1 = parse_position(text);
    if (!p1)
        return fail_parse<Move>();
    auto p2 = parse_position((*p1).second);
    if (!p2)
        return fail_parse<Move>();
    return pair<Move, string>(mkMove((*p1).first, (*p2).first), (*p2).second);
}

optional<pair<string, string> > parse_until(const string& text, function<bool(char)> pred) {
    auto matching_text = string();
    auto remaining_text = text;
    auto at_least_once = false;
    while (auto o_pair = parse_value<char>(remaining_text)) {
        auto _pair = *o_pair;
        if (pred(_pair.first))
            break;
        at_least_once = true;
        matching_text += _pair.first;
        remaining_text = _pair.second;
    }
    if (!at_least_once)
        return fail_parse<string>();
    return pair<string, string>(matching_text, remaining_text);
   
}

optional<pair<string, string> > parse_token(const string& text, char delimiter) {
    auto parsed = parse_until(text, [&] (char c) {
        return c == delimiter;
    });
    if (!parsed)
        return fail_parse<string>();
    auto result = (*parsed).first;
    auto maybe_eat_delimiter = parse_value<char>((*parsed).second);
    if (!maybe_eat_delimiter)
        return parsed;
    return pair<string, string>(result, (*maybe_eat_delimiter).second);
}
