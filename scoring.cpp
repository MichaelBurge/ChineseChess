#include "scoring.hpp"
#include <stdexcept>
using namespace std;

int piece_value(PieceType piece_type) {
    switch (piece_type) {
    case GENERAL:
        return 1000000;
    case ADVISOR:
        return 1000;
    case CHARIOT:
        return 10000;
    case ELEPHANT:
        return 3000;
    case SOLDIER:
        return 500;
    case HORSE:
        return 5000;
    case CANNON:
        return 6000;
    default:
        throw logic_error("Unknown piece");
    }
}

int piece_score(const GameState& state) {
    int accum = 0;
    for_each_piece(state, [&] (const Position& position, const Piece& piece) {
        int value = piece_value(piece.piece_type);
        if (piece.owner != RED)
            value *= -1;
        accum += value;
    });
    return accum;
}
