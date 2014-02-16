#include "player.hpp"

Player next_player(Player player) {
    return player == RED
        ? BLACK
        : RED;
}
