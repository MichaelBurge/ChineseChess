#include "piece.hpp"
#include <boost/lexical_cast.hpp>
using namespace boost;

char character_for_piece(Piece piece) {
    switch (piece) {
    case EMPTY:
	return '.';
    case RED_GENERAL:
	return 'G';
    case BLACK_GENERAL:
	return 'g';
    case RED_ADVISOR:
	return 'A';
    case BLACK_ADVISOR:
	return 'a';
    case RED_ELEPHANT:
	return 'E';
    case BLACK_ELEPHANT:
	return 'e';
    case RED_HORSE:
	return 'H';
    case BLACK_HORSE:
	return 'h';
    case RED_CHARIOT:
	return 'R';
    case BLACK_CHARIOT:
	return 'r';
    case RED_CANNON:
	return 'N';
    case BLACK_CANNON:
	return 'n';
    case RED_SOLDIER:
	return 'S';
    case BLACK_SOLDIER:
	return 's';
    default:
        throw logic_error("Unknown piece: " + lexical_cast<string>((int)piece));
    }
}

ostream& operator<<(ostream& os, const Piece& piece) {
    return os << character_for_piece(piece);
}
