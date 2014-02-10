#include "move.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

Move::Move(const Position& from, const Position& to) : from(from), to(to) { }
Move::Move(const Position& from, Direction direction) : from(from), to(move_direction(from, direction)) { }

string move_repr(const Move& move) {
    return position_repr(move.from) + position_repr(move.to);
}

ostream& operator<<(ostream& os, const Move& move) {
    return os << move_repr(move);
}

bool Move::operator<(const Move& b) const {
    return (from < b.from) ||
            (from == b.from &&
             to < b.to);
}

bool Move::operator==(const Move& b) const {
    return this->from == b.from && this->to == b.to;
}

void print_moves(const vector<Move>& moves) {
    if (moves.empty())
        cout << "No moves available!" << endl;
    else
        for_each(moves.begin(), moves.end(), [] (const Move& move) {
            cout << move << endl;
        });
}
