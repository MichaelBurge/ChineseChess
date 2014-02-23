#include "bitboard-gamestate.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <cassert>
#include <iostream>
using namespace boost;
using namespace std;

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
    clear_cached_data();
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
    case EMPTY:
	throw logic_error("Attempted to insert an empty piece");
    default:
	throw logic_error("Unknown piece");
    }
    assert(check_internal_consistency());
}

void BitboardGameState::remove_piece(const Position& position) {
    if (get_piece(position) == EMPTY)
	throw logic_error("Tried to remove an empty piece");
    auto index = position.value;
    all_pieces.  clear(index);
    red_pieces.  clear(index);
    black_pieces.clear(index);
    generals.    clear(index);
    advisors.    clear(index);
    elephants.   clear(index);
    horses.      clear(index);
    chariots.    clear(index);
    cannons.     clear(index);
    soldiers.    clear(index);
    clear_cached_data();
    assert(check_internal_consistency());
}

void BitboardGameState::switch_turn() {
    current_turn(next_player(current_turn()));
}

void BitboardGameState::clear_cached_data() {
    is_cache_valid = false;
}

void BitboardGameState::apply_move(const Move& move) {
    Piece from_piece = get_piece(move.from);
    // TODO: There are about 5 get_piece calls hidden here - performance problem?
    if (get_piece(move.to) != EMPTY)
	remove_piece(move.to);
    remove_piece(move.from);
    insert_piece(move.to, from_piece);
    switch_turn();

    check_internal_consistency();
}

void BitboardGameState::peek_move(const Move& move, const function<void(const BitboardGameState&)>& action) const {
    Piece to_piece = get_piece(move.to);
    auto& self = const_cast<BitboardGameState&>(*this);
    self.apply_move(move);
    action(self);
    self.apply_move(Move(move.to, move.from));
    if (to_piece)
	self.insert_piece(move.to, to_piece);

    check_internal_consistency();
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
    cout << "Pieces list:" << endl;
}

void BitboardGameState::for_each_red_piece(function<void(Position, Piece)> action) const {
    uint8_t position;
    bitboard candidates;

    ITER_PIECES(chariots & red_pieces, RED_CHARIOT, action);
    ITER_PIECES(cannons & red_pieces, RED_CANNON, action);
    ITER_PIECES(horses & red_pieces, RED_HORSE, action);
    ITER_PIECES(soldiers & red_pieces, RED_SOLDIER, action);
    ITER_PIECES(advisors & red_pieces, RED_ADVISOR, action);
    ITER_PIECES(generals & red_pieces, RED_GENERAL, action);
    ITER_PIECES(elephants & red_pieces, RED_ELEPHANT, action);
}

void BitboardGameState::for_each_black_piece(function<void(Position, Piece)> action) const {
    uint8_t position;
    bitboard candidates;

    ITER_PIECES(chariots & black_pieces, BLACK_CHARIOT, action);
    ITER_PIECES(cannons & black_pieces, BLACK_CANNON, action);
    ITER_PIECES(horses & black_pieces, BLACK_HORSE, action);
    ITER_PIECES(soldiers & black_pieces, BLACK_SOLDIER, action);
    ITER_PIECES(advisors & black_pieces, BLACK_ADVISOR, action);
    ITER_PIECES(generals & black_pieces, BLACK_GENERAL, action);
    ITER_PIECES(elephants & black_pieces, BLACK_ELEPHANT, action);
}

void BitboardGameState::for_each_piece(function<void(Position, Piece)> action) const {
    for_each_red_piece(action);
    for_each_black_piece(action);
}

bool BitboardGameState::check_internal_consistency() const {
#ifndef ENABLE_DEBUG_CONSISTENCY_CHECKS
    return true;
#endif


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
	    raise_error(a, b, message);	}
    };

    assert_equal(all_pieces, red_pieces | black_pieces, "All pieces & (red | black)");

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
