#include "scoring.hpp"
#include <stdexcept>
using namespace std;

int piece_value(Piece piece) {
    switch (piece) {
    case RED_GENERAL:
    case BLACK_GENERAL:
        return 1000000;
    case RED_ADVISOR:
    case BLACK_ADVISOR:
        return 1000;
    case RED_CHARIOT:
    case BLACK_CHARIOT:
        return 10000;
    case RED_ELEPHANT:
    case BLACK_ELEPHANT:
        return 2000;
    case RED_SOLDIER:
    case BLACK_SOLDIER:
        return 500;
    case RED_HORSE:
    case BLACK_HORSE:
        return 3000;
    case RED_CANNON:
    case BLACK_CANNON:
        return 5000;
    default:
        throw logic_error("Unknown piece");
    }
}

int piece_score(const StandardGameState& state) {
    int accum = 0;
    state.for_each_piece([&] (const Position& position, const Piece& piece) {
        int value = piece_value(piece);
        if (owner(piece) != state.current_turn())
            value *= -1;
        accum += value;
    });
    return accum;
}

int standard_score_function(const StandardGameState& state) {
    return piece_score(state);
}
