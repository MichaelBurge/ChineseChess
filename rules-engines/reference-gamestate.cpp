#include "reference-gamestate.hpp"
#include "../gamestate.hpp"
#include "../position.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

using namespace boost;
using namespace std;

ReferenceGameState::ReferenceGameState(Player _current_turn) : turn(_current_turn), pieces_array(), pieces_map() , undo_stack() { }

UndoNode::UndoNode(const ReferenceGameState& state, const Move& move) : move(move) {
    this->former_occupant = state.get_piece(move.to);
}

// ReferenceGameStateArrayStorage

ReferenceGameStateArrayStorage::ReferenceGameStateArrayStorage() : pieces(extents[10][9]) {
    pieces.reindex(1);
    fill(pieces.data(), pieces.data() + pieces.num_elements(), Piece(EMPTY));
}

optional<Piece> ReferenceGameStateArrayStorage::get_piece(const Position& position) const {
    if (!position.is_valid())
	return optional<Piece>();
    auto piece = this->get_piece_direct(position);
    return (piece == EMPTY)
	? optional<Piece>()
        : piece;  
};

optional<Position> ReferenceGameStateArrayStorage::get_position(const Piece& piece) const {
    throw logic_error("Calling ReferenceGameStateArrayStorage::get_piece(piece) is a performance error");
};

Piece ReferenceGameStateArrayStorage::get_piece_direct(const Position& position) const {
    if (!position.is_valid())
	throw logic_error("Invalid position" + position_repr(position));
    return pieces[position.rank][position.file];

}

void ReferenceGameStateArrayStorage::insert_piece(const Position& position, const Piece& piece) {
    if (!position.is_valid())
	throw logic_error("Invalid position " + position_repr(position));
    this->pieces[position.rank][position.file] = piece;
}

void ReferenceGameStateArrayStorage::for_each_piece(function<void(Position, Piece)> action) const {
    for (int rank = 1; rank <= 10; rank++) {
	for (int file = 1; file <= 9; file++) {
	    auto position = Position(rank, file);
	    auto piece = this->get_piece(position);
	    if (!piece)
		continue;
	    action(position, *piece);
	}
    }
}

void ReferenceGameStateArrayStorage::remove_piece(const Position& position) {
    auto piece = this->get_piece(position);
    if (!piece)
	throw logic_error("Tried to remove a nonexistent piece");
    this->pieces[position.rank][position.file] = Piece(EMPTY);
}

// ReferenceGameStateDictionaryStorage

ReferenceGameStateDictionaryStorage::ReferenceGameStateDictionaryStorage() : pieces() { }

optional<Piece> ReferenceGameStateDictionaryStorage::get_piece(const Position& position) const {
    auto p = this->pieces.left.find(position);
    return (p == this->pieces.left.end())
	? optional<Piece>()
	: (*p).second;
}

optional<Position> ReferenceGameStateDictionaryStorage::get_position(const Piece& piece) const {
    auto p = this->pieces.right.find(piece);
    return (p == this->pieces.right.end())
	? optional<Position>()
	: (*p).second;
};

void ReferenceGameStateDictionaryStorage::insert_piece(const Position& position, const Piece& piece) {
    if (!position.is_valid())
	throw logic_error("Invalid position " + position_repr(position));
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

optional<Piece> ReferenceGameState::get_piece(const Position& position) const {
    return this->pieces_array.get_piece(position);
}

optional<Position> ReferenceGameState::get_position(const Piece& piece) const {
    return this->pieces_map.get_position(piece);
}

void ReferenceGameState::insert_piece(const Position& position, const Piece& piece) {
    this->pieces_array.insert_piece(position, piece);
    this->pieces_map.insert_piece(position, piece);
}

void ReferenceGameState::remove_piece(const Position& position) {
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
    this->insert_piece(move.to, *from_piece);
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

string player_repr(Player player) {
    if (player == RED)
        return "Red";
    else
        return "Black";
}

ostream& operator<<(ostream& os, const ReferenceGameStateArrayStorage& board) {
    auto draw_river = [] () {
	for (int i = 1; i != 9; i++)
	    cout << "~~";
        cout << "~" << endl;
    };

    auto draw_rank = [&] (int rank) {
	for (int i = 1; i != 10; i++)
            cout << character_for_piece(board.get_piece_direct(Position(rank, i))) << ' ';
        cout << endl;
    };

    for (int i = 10; i != 0; i--) {
        if (i == 5)
            draw_river();
        draw_rank(i);
    };
    return os;
}

void ReferenceGameState::print_debug_board() const {
    cout << *this;
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

ostream& operator<<(ostream& os, const ReferenceGameState& state) {
    os << "Current Turn: " << player_repr(state.current_turn()) << endl;
    os << state.pieces_array << endl;
    return os;
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
    this->insert_piece(move.from, *piece);
    if (!!node.former_occupant)
	this->insert_piece(move.to, *node.former_occupant);

    this->switch_turn();
}

Player ReferenceGameState::current_turn() const {
    return this->turn;
}

void ReferenceGameState::current_turn(Player player) {
    this->turn = player;
}
