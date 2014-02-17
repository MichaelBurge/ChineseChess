#include "reference-gamestate.hpp"
#include "../gamestate.hpp"
#include "../position.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

using namespace boost;
using namespace std;

ReferenceGameState::ReferenceGameState(Player _current_turn) :
    red_king(optional<Position>()),
    black_king(optional<Position>()),
    turn(_current_turn),
    pieces_array(),
    pieces_map() ,
    undo_stack() { }

UndoNode::UndoNode(const ReferenceGameState& state, const Move& move) : move(move) {
    this->former_occupant = state.get_piece(move.to);
}

// ReferenceGameStateArrayStorage

ReferenceGameStateArrayStorage::ReferenceGameStateArrayStorage() : pieces(extents[10][9]) {
    pieces.reindex(1);
    fill(pieces.data(), pieces.data() + pieces.num_elements(), Piece(EMPTY));
}

Piece ReferenceGameStateArrayStorage::get_piece(const Position& position) const {
    if (!position.is_valid())
	throw logic_error("Invalid position" + position_repr(position));
    return pieces[position.rank()][position.file()];
};

void ReferenceGameStateArrayStorage::insert_piece(const Position& position, const Piece& piece) {
    if (!position.is_valid())
	throw logic_error("Invalid position " + position_repr(position));
    this->pieces[position.rank()][position.file()] = piece;
}

void ReferenceGameStateArrayStorage::for_each_piece(function<void(Position, Piece)> action) const {
    for (int rank = 1; rank <= 10; rank++) {
	for (int file = 1; file <= 9; file++) {
	    auto position = Position(rank, file);
	    auto piece = this->get_piece(position);
	    if (!piece)
		continue;
	    action(position, piece);
	}
    }
}

void ReferenceGameStateArrayStorage::remove_piece(const Position& position) {
    auto piece = this->get_piece(position);
    if (!piece)
	throw logic_error("Tried to remove a nonexistent piece");
    this->pieces[position.rank()][position.file()] = Piece(EMPTY);
}

// ReferenceGameStateDictionaryStorage

ReferenceGameStateDictionaryStorage::ReferenceGameStateDictionaryStorage() : pieces() { }

Piece ReferenceGameStateDictionaryStorage::get_piece(const Position& position) const {
    auto p = this->pieces.left.find(position);
    return (p == this->pieces.left.end())
	? EMPTY
	: (*p).second;
}

void ReferenceGameStateDictionaryStorage::insert_piece(const Position& position, const Piece& piece) {
    if (!position.is_valid())
	throw logic_error("Invalid position " + position_repr(position));
    if (piece == EMPTY)
	throw logic_error("Tried to insert an empty piece at " + position_repr(position));
    this->pieces.right.insert(bimap<Position, Piece>::right_value_type(piece, position));
}

void ReferenceGameStateDictionaryStorage::for_each_piece(function<void(Position, Piece)> action) const {
    typedef typename bimap<Position, Piece>::const_iterator const_iterator;
    for(const_iterator iter = this->pieces.begin(); iter != this->pieces.end(); iter++) {
	action(iter->left, iter->right);
    }
}

void ReferenceGameStateDictionaryStorage::remove_piece(const Position& position) {
    this->pieces.left.erase(position);
}

int ReferenceGameStateDictionaryStorage::size() const {
    return this->pieces.size();
}

ostream& operator<<(ostream& os, const ReferenceGameStateDictionaryStorage& x) {
    x.for_each_piece([&] (Position position, Piece piece) {
	    os << "Position: ";
	    os << position;
	    os << ", Piece: ";
	    os << piece << endl;
    });
    return os;
}

// ReferenceGameState

void ReferenceGameState::peek_move(const Move& move, const function<void(const ReferenceGameState&)>& action) const {
    auto& scratch = const_cast<ReferenceGameState&>(*this);
    scratch.undo_stack.push_back(UndoNode(scratch, move));
    scratch.apply_move(move);
    action(scratch);
    scratch.rollback();
}

Piece ReferenceGameState::get_piece(const Position& position) const {
    return this->pieces_array.get_piece(position);
}

void ReferenceGameState::insert_piece(const Position& position, const Piece& piece) {
    if (piece == EMPTY)
	throw logic_error("Cannot insert an empty piece");
    if (piece == RED_GENERAL)
	this->red_king = position;
    if (piece == BLACK_GENERAL)
	this->black_king = position;
    this->pieces_array.insert_piece(position, piece);
    this->pieces_map.insert_piece(position, piece);
}

void ReferenceGameState::remove_piece(const Position& position) {
    if (!!this->red_king && *red_king == position)
	this->red_king = optional<Position>();
    if (!!this->black_king && *black_king == position)
	this->black_king = optional<Position>();
    this->pieces_array.remove_piece(position);
    this->pieces_map.remove_piece(position);
}

void ReferenceGameState::for_each_piece(function<void(Position, Piece)> action) const {
    this->pieces_map.for_each_piece(action);
}

void ReferenceGameState::switch_turn() {
    this->current_turn(next_player(this->current_turn()));    
}

void ReferenceGameState::apply_move(const Move& move) {
    auto from_piece = this->get_piece(move.from);
    auto to_piece = this->get_piece(move.to);
    if (!from_piece)
        throw logic_error("No piece in the 'from' coordinate of this move");
    this->remove_piece(move.from);
    if (!!to_piece)
	this->remove_piece(move.to);
    this->insert_piece(move.to, from_piece);
    if (move.from == this->red_king)
	this->red_king = move.to;
    if (move.from == this->black_king)
	this->black_king = move.to;
    this->switch_turn();
}


vector<Position> ReferenceGameState::filter_pieces(function<bool(Position, Piece)> pred) const {
    auto matches = vector<Position>();
    this->for_each_piece([&] (Position position, Piece piece) {
        if (pred(position, piece))
            matches.push_back(position);
    });
    return matches;
}

void ReferenceGameState::print_debug_board() const {
    cout << GameState<ReferenceGameState>(*this) << endl;
    cout << "Pieces map: " << endl;
    cout << this->pieces_map << endl;
    cout << "Debug info: " << endl;
    this->print_undo_stack();
}

void ReferenceGameState::print_undo_stack() const {
    cout << "Undo stack:" << endl;
    for (const UndoNode& undo : this->undo_stack) {
	cout << undo << endl;
    }
}

ostream& operator<<(ostream& os, const UndoNode& undo) {
    return os << "Undo(" << undo.move << "," << undo.former_occupant << ")";
}

void ReferenceGameState::rollback() {
    if (this->undo_stack.empty())
	throw logic_error("Tried to rollback with an empty undo stack");
    auto node = this->undo_stack.back();
    this->undo_stack.pop_back();
    auto move = node.move;
    auto piece = this->get_piece(move.to);
    if (!piece)
	throw logic_error("No piece at to location for move " + move_repr(move));

    this->remove_piece(move.to);
    this->insert_piece(move.from, piece);
    if (!!node.former_occupant && *node.former_occupant)
	this->insert_piece(move.to, *node.former_occupant);

    this->switch_turn();
}

Player ReferenceGameState::current_turn() const {
    return this->turn;
}

void ReferenceGameState::current_turn(Player player) {
    this->turn = player;
}

optional<Position> ReferenceGameState::get_king_position(Player player) const {
    return (player == RED)
	? this->red_king
	: this->black_king;
}

