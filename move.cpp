#include "move.hpp"
#include "parsing.hpp"
#include <algorithm>
#include <iostream>
#include <boost/optional/optional.hpp>
using namespace std;
using namespace boost;

string move_repr(const Move& move) {
    return position_repr(move.from) + position_repr(move.to);
}

ostream& operator<<(ostream& os, const Move& move) {
    return os << move_repr(move);
}

void print_moves(const vector<Move>& moves) {
    if (moves.empty())
        cout << "No moves available!" << endl;
    else
        for_each(moves.begin(), moves.end(), [] (const Move& move) {
            cout << move << endl;
        });
}

template<> optional<pair<Move, string> > parse_value<Move>(const string& text) {
    auto p1 = parse_value<Position>(text);
    if (!p1)
        return fail_parse<Move>();
    auto p2 = parse_value<Position>((*p1).second);
    if (!p2)
        return fail_parse<Move>();
    return pair<Move, string>(Move((*p1).first, (*p2).first), (*p2).second);
}

Move::Move(const string& text) {
    auto parsed = parse_value<Move>(text);
    if (!parsed)
	throw runtime_error("Not a move: " + text);
    *this = (*parsed).first;
}
