#include "bitboard-gamestate.hpp"
#include <stdexcept>
using namespace std;

ostream& operator<<(ostream& os, const BitboardGameState& state) {
    throw runtime_error("Unimplemented");
}

Piece BitboardGameState::get_piece(const Position&) const {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::insert_piece(const Position&, const Piece&) {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::remove_piece(const Position&) {
    throw runtime_error("Unimplemented");
}

void BitboardGameState::apply_move(const Move&) {
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
