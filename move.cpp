#include "move.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

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
