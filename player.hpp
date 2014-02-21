#pragma once

#include <string>
using namespace std;

enum Player { RED = 0, BLACK = 1};
extern Player next_player(Player);
extern string player_repr(Player player);
