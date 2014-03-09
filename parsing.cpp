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
