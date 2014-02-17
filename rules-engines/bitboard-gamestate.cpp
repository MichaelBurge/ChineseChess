#include "bitboard-gamestate.hpp"
#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <iostream>
using namespace std;

ostream& operator<<(ostream& os, const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

Piece BitboardGameState::get_piece(const Position& position) const {
    bitboard x;
    x.set(position.value);
    bool is_red = !!(red_pieces & x);
    bool is_black = !!(black_pieces & x);
    if (!is_red && !is_black)
	return EMPTY;
    if (!!(generals & x))
	return is_red ? RED_GENERAL : BLACK_GENERAL;
    if (!!(soldiers & x))
	return is_red ? RED_SOLDIER : BLACK_SOLDIER;
    if (!!(advisors & x))
	return is_red ? RED_ADVISOR : BLACK_ADVISOR;
    if (!!(elephants & x))
	return is_red ? RED_ELEPHANT : BLACK_ELEPHANT;
    if (!!(horses & x))
	return is_red ? RED_HORSE : BLACK_HORSE;
    if (!!(chariots & x))
	return is_red ? RED_CHARIOT : BLACK_CHARIOT;
    if (!!(cannons & x))
	return is_red ? RED_CANNON : BLACK_CANNON;
    throw logic_error("Program should not have reached here");
}

void BitboardGameState::insert_piece(const Position& position, const Piece& piece) {
    // TODO: Maybe use push_back or push_front depending on the piece?
    pieces.push_back(Cell(position, piece));
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
    throw runtime_error("Unimplemented");
}

void BitboardGameState::current_turn(Player) {
    throw runtime_error("Unimplemented");
}

Player BitboardGameState::current_turn() const {
    return _current_turn;
    throw runtime_error("Unimplemented");
}

void BitboardGameState::peek_move(const Move&, const function<void(const BitboardGameState&)>& action) const {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::print_debug_board() const {
    cout << "Current turn: " << player_repr(current_turn()) << endl;
    cout << "Red pieces:" << endl;
    print_bitboard(cout, red_pieces);
    cout << "Black pieces:" << endl;
    print_bitboard(cout, black_pieces);
    cout << "Generals:" << endl;
    print_bitboard(cout, generals);
    cout << "Advisors:" << endl;
    print_bitboard(cout, advisors);
    cout << "Elephants:" << endl;
    print_bitboard(cout, elephants);
    cout << "Cannons:" << endl;
    print_bitboard(cout, cannons);
    cout << "Chariots:" << endl;
    print_bitboard(cout, chariots);
    cout << "Soldiers:" << endl;
    print_bitboard(cout, soldiers);
}

void BitboardGameState::for_each_piece(function<void(Position, Piece)> action) const {
    for (const Cell& cell : pieces) {
	action(cell.position, cell.piece);
    }
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
