#pragma once

#include "player.hpp"
#include <ostream>
#include <stdexcept>
using namespace std;

enum Piece {
    EMPTY = 0,
    RED_GENERAL = 1,
    RED_ADVISOR = 2,
    RED_ELEPHANT = 3,
    RED_HORSE = 4,
    RED_CHARIOT = 5,
    RED_CANNON = 6,
    RED_SOLDIER = 7,
    BLACK_GENERAL = 9,
    BLACK_ADVISOR = 10,
    BLACK_ELEPHANT = 11,
    BLACK_HORSE = 12,
    BLACK_CHARIOT = 13,
    BLACK_CANNON = 14,
    BLACK_SOLDIER = 15,
};

extern Player owner(Piece piece);
extern char character_for_piece(Piece piece);
extern ostream& operator<<(ostream& os, const Piece& piece);
