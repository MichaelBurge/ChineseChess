#include "player.hpp"

Player next_player(Player player) {
    return player == RED
        ? BLACK
        : RED;
}

string player_repr(Player player) {
    if (player == RED)
        return "Red";
    else
        return "Black";
}
