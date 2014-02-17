#include "bitboard-gamestate.hpp"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <iostream>
using namespace std;

ostream& operator<<(ostream& os, const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

Piece BitboardGameState::get_piece(const Position& ) const {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::insert_piece(const Position& position, const Piece& piece) {
    uint8_t index = position.value;
    all_pieces.set(index);
    switch (piece) {
    case RED_GENERAL:
	generals.set(index);
	red_pieces.set(index);
	break;
    case BLACK_GENERAL:
	generals.set(index);
	black_pieces.set(index);
	break;
    case RED_ADVISOR:
	advisors.set(index);
	red_pieces.set(index);
	break;
    case BLACK_ADVISOR:
	advisors.set(index);
	black_pieces.set(index);
	break;
    case RED_ELEPHANT:
	elephants.set(index);
	red_pieces.set(index);
	break;
    case BLACK_ELEPHANT:
	elephants.set(index);
	black_pieces.set(index);
	break;
    case RED_HORSE:
	horses.set(index);
	red_pieces.set(index);
	break;
    case BLACK_HORSE:
	horses.set(index);
	black_pieces.set(index);
	break;
    case RED_CHARIOT:
	chariots.set(index);
	red_pieces.set(index);
	break;
    case BLACK_CHARIOT:
	chariots.set(index);
	black_pieces.set(index);
	break;
    case RED_CANNON:
	cannons.set(index);
	red_pieces.set(index);
	break;
    case BLACK_CANNON:
	cannons.set(index);
	black_pieces.set(index);
	break;
    case RED_SOLDIER:
	soldiers.set(index);
	red_pieces.set(index);
	break;
    case BLACK_SOLDIER:
	soldiers.set(index);
	black_pieces.set(index);
	break;
    default:
	throw logic_error("Unknown piece");
    }
    assert(check_internal_consistency());
}

void BitboardGameState::remove_piece(const Position& position) {
    auto index = position.value;
    red_pieces.  clear(index);
    black_pieces.clear(index);
    generals.    clear(index);
    advisors.    clear(index);
    elephants.   clear(index);
    horses.      clear(index);
    chariots.    clear(index);
    cannons.     clear(index);
    soldiers.    clear(index);
    pieces.erase(find_if(pieces.begin(), pieces.end(), [&] (const Cell& cell) {
	return cell.position == position;
    }));
    assert(check_internal_consistency());
}

void BitboardGameState::apply_move(const Move& move) {
    // Remember to flip the board, since soldier & elephant moves assume red.
    throw runtime_error("Unimplemented");
}

void BitboardGameState::current_turn(Player) {
    throw runtime_error("Unimplemented");
}

Player BitboardGameState::current_turn() const {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::peek_move(const Move&, const function<void(const BitboardGameState&)>& action) const {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::print_debug_board() const {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::for_each_piece(function<void(Position, Piece)> action) const {
    throw runtime_error("Unimplemented");
}

bool BitboardGameState::check_internal_consistency() const {

    auto raise_error = [] (const bitboard& a, const bitboard& b, string message) {
	cerr << "Assertion failure: " << message << endl;
	cerr << "a: " << a << endl;
	print_bitboard(cerr, a);
	cerr << "b: " << b << endl;
	print_bitboard(cerr, b);
	throw logic_error("Internal consistency check failed: " + message);
    };

    auto assert_equal = [&] (const bitboard& a, const bitboard& b, string message) {
	if (a == b)
	    return;
	else
	    raise_error(a, b, message);
    };

    auto assert_no_overlap = [&] (const bitboard& a, const bitboard& b, string message) {
	if ((a & b) == uint128_t(0, 0)) {
	    return;
	} else {
	    raise_error(a, b, message);
	}
    };
    assert_equal(
        red_pieces | black_pieces,
	generals | advisors | elephants | horses | chariots | cannons | soldiers,
	"Pieces viewed from different boards don't add up");

    assert_no_overlap(red_pieces, black_pieces, "Red & Black pieces");
	
    assert_no_overlap(generals, advisors,  "Generals & Advisors");
    assert_no_overlap(generals, elephants, "Generals & Elephants");
    assert_no_overlap(generals, horses,    "Generals & Horses");
    assert_no_overlap(generals, chariots,  "Generals & Chariots");
    assert_no_overlap(generals, cannons,   "Generals & Cannons");
    assert_no_overlap(generals, soldiers,  "Generals & Soldiers");

    assert_no_overlap(advisors, elephants, "Advisors & Elephants");
    assert_no_overlap(advisors, horses,    "Advisors & Horses");
    assert_no_overlap(advisors, chariots,  "Advisors & Chariots");
    assert_no_overlap(advisors, cannons,   "Advisors & Cannons");
    assert_no_overlap(advisors, soldiers,  "Advisors & Soldiers");

    assert_no_overlap(elephants, horses,   "Elephants & Horses");  
    assert_no_overlap(elephants, chariots, "Elephants & Chariots");
    assert_no_overlap(elephants, cannons,  "Elephants & Cannons");
    assert_no_overlap(elephants, soldiers, "Elephants & Soldiers");

    assert_no_overlap(horses, chariots,    "Horses & Chariots");
    assert_no_overlap(horses, cannons,     "Horses & Cannons");
    assert_no_overlap(horses, soldiers,    "Horses & Soldiers");

    assert_no_overlap(chariots, cannons,   "Chariots & Cannons");
    assert_no_overlap(chariots, soldiers,  "Chariots & Soldiers");
    
    assert_no_overlap(cannons, soldiers,   "Cannons & Soldiers");

    return true;
}
