#pragma once

#include <string>
using namespace std;

enum Player { RED = 0, BLACK = 1};

inline Player next_player(Player player) {
    return player == RED
        ? BLACK
        : RED;
}

inline string player_repr(Player player) {
    if (player == RED)
        return "Red";
    else
        return "Black";
}
