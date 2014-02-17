#pragma once

#include <string>
using namespace std;

enum Player { RED, BLACK };
extern Player next_player(Player);
extern string player_repr(Player player);
