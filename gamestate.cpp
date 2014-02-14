#include "gamestate.hpp"
#include "position.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

using namespace boost;
using namespace std;

Player next_player(Player player) {
    return player == RED
        ? BLACK
        : RED;}


GameState::GameState(Player _current_turn) : turn(_current_turn), pieces_array(), pieces_map() , undo_stack() { }

UndoNode::UndoNode(const GameState& state, const Move& move) : move(move) {
    this->former_occupant = state.get_piece(move.to);
}

// Piece
Piece::Piece(PieceType piece_type) : piece_type(piece_type), owner(RED), unique_id(0) {
    if (piece_type != EMPTY)
	throw logic_error("Non-empty pieces require an owner");
}
Piece::Piece(PieceType piece_type, Player owner) : piece_type(piece_type), owner(owner), unique_id(0) {
    if (piece_type != GENERAL)
	throw logic_error("Non-generals require a uniquifier");
}

Piece::Piece(PieceType piece_type, Player owner, int unique_id) : piece_type(piece_type), owner(owner), unique_id(unique_id) {
    if (unique_id < 0)
	throw logic_error("Unique id cannot be negative");
    if (piece_type == SOLDIER) {
	if (unique_id >= 5)
	    throw logic_error("Only 5 soldiers allowed");
    } else if (unique_id >= 2)
	throw logic_error("Only 2 pieces allowed");
}

Piece::Piece() { }

char Piece::character() const {
    auto c = '\a';
    switch (this->piece_type) {
    case EMPTY:
        c = '.'; break;
    case GENERAL:
        c = 'g'; break;
    case ADVISOR:
        c = 'a'; break;
    case ELEPHANT:
        c = 'e'; break;
    case HORSE:
        c = 'h'; break;
    case CHARIOT:
        c = 'r'; break;
    case CANNON:
        c = 'n'; break;
    case SOLDIER:
        c = 's'; break;
    default:
        throw logic_error("Unknown piece: " + lexical_cast<string>(this->piece_type));
    }
    if (this->owner == RED)
        c = toupper(c);
    return c;
}

bool Piece::operator<(const Piece& b) const {
    return owner < b.owner ||
          (owner == b.owner &&
	   (piece_type < b.piece_type ||
	    (piece_type == b.piece_type &&
	     unique_id < b.unique_id)));
}

bool Piece::operator==(const Piece& b) const {
    return piece_type == b.piece_type && owner == b.owner;
}


ostream& operator<<(ostream& os, const Piece& piece) {
    return os << piece.character();
}

// GameStateArrayStorage

GameStateArrayStorage::GameStateArrayStorage() : pieces(extents[10][9]) {
    pieces.reindex(1);
    fill(pieces.data(), pieces.data() + pieces.num_elements(), Piece(EMPTY));
}

optional<Piece> GameStateArrayStorage::get_piece(const Position& position) const {
    if (!position.is_valid())
	return optional<Piece>();
    auto p = this->get_piece_direct(position);
    return (p.piece_type == EMPTY)
	? optional<Piece>()
        : p;    
};

optional<Position> GameStateArrayStorage::get_position(const Piece& piece) const {
    throw logic_error("Calling GameStateArrayStorage::get_piece(piece) is a performance error");
};

Piece GameStateArrayStorage::get_piece_direct(const Position& position) const {
    if (!position.is_valid())
	throw logic_error("Invalid position" + position_repr(position));
    return pieces[position.rank][position.file];

}

void GameStateArrayStorage::insert_piece(const Position& position, const Piece& piece) {
    if (!position.is_valid())
	throw logic_error("Invalid position " + position_repr(position));
    this->pieces[position.rank][position.file] = piece;
}

void GameStateArrayStorage::for_each_piece(function<void(Position, Piece)> action) const {
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

void GameStateArrayStorage::remove_piece(const Position& position) {
    auto piece = this->get_piece(position);
    if (!piece)
	throw logic_error("Tried to remove a nonexistent piece");
    this->pieces[position.rank][position.file] = Piece(EMPTY);
}

// GameStateDictionaryStorage

GameStateDictionaryStorage::GameStateDictionaryStorage() : pieces() { }

optional<Piece> GameStateDictionaryStorage::get_piece(const Position& position) const {
    auto p = this->pieces.left.find(position);
    return (p == this->pieces.left.end())
	? optional<Piece>()
	: (*p).second;
}

optional<Position> GameStateDictionaryStorage::get_position(const Piece& piece) const {
    auto p = this->pieces.right.find(piece);
    return (p == this->pieces.right.end())
	? optional<Position>()
	: (*p).second;
};

void GameStateDictionaryStorage::insert_piece(const Position& position, const Piece& piece) {
    if (!position.is_valid())
	throw logic_error("Invalid position " + position_repr(position));
    this->pieces.right.insert(bimap<Position, Piece>::right_value_type(piece, position));
}

void GameStateDictionaryStorage::for_each_piece(function<void(Position, Piece)> action) const {
    typedef typename bimap<Position, Piece>::const_iterator const_iterator;
    for(const_iterator iter = this->pieces.begin(); iter != this->pieces.end(); iter++) {
	action(iter->left, iter->right);
    }
}

void GameStateDictionaryStorage::remove_piece(const Position& position) {
    this->pieces.left.erase(position);
}

int GameStateDictionaryStorage::size() const {
    return this->pieces.size();
}

ostream& operator<<(ostream& os, const GameStateDictionaryStorage& x) {
    x.for_each_piece([&] (Position position, Piece piece) {
	    os << "Position: ";
	    os << position;
	    os << ", Piece: ";
	    os << piece << endl;
    });
    return os;
}

// GameState

template<typename T> T GameState::peek_move(Move move, const function<T(const GameState &)>& action) const {
    auto& scratch = const_cast<GameState&>(*this);
    scratch.undo_stack.push_back(UndoNode(scratch, move));
    scratch.apply_move(move);
    auto ret = action(scratch);
    scratch.rollback();
    return ret;
}

template int GameState::peek_move<int>(Move, const function<int(const GameState &)>&) const;
template bool GameState::peek_move<bool>(Move, const function<bool(const GameState &)>&) const;

template<> void GameState::peek_move(Move move, const function<void(const GameState &)>& action) const {
    peek_move<bool>(move, [&] (const GameState& state) {
	action(state);
	return true;
    });
}


optional<Piece> GameState::get_piece(const Position& position) const {
    return this->pieces_array.get_piece(position);
}

optional<Position> GameState::get_position(const Piece& piece) const {
    return this->pieces_map.get_position(piece);
}

void GameState::insert_piece(const Position& position, const Piece& piece) {
    this->pieces_array.insert_piece(position, piece);
    this->pieces_map.insert_piece(position, piece);
}

void GameState::remove_piece(const Position& position) {
    this->pieces_array.remove_piece(position);
    this->pieces_map.remove_piece(position);
}

void GameState::for_each_piece(function<void(Position, Piece)> action) const {
    this->pieces_map.for_each_piece(action);
}



void GameState::switch_turn() {
    this->current_turn(next_player(this->current_turn()));    
}

void GameState::apply_move(const Move& move) {
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


vector<Position> GameState::filter_pieces(function<bool(Position, Piece)> pred) const {
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

ostream& operator<<(ostream& os, const GameStateArrayStorage& board) {
    auto draw_river = [] () {
	for (int i = 1; i != 9; i++)
	    cout << "~~";
        cout << "~" << endl;
    };

    auto draw_rank = [&] (int rank) {
	for (int i = 1; i != 10; i++)
            cout << board.get_piece_direct(Position(rank, i)).character() << ' ';
        cout << endl;
    };

    for (int i = 10; i != 0; i--) {
        if (i == 5)
            draw_river();
        draw_rank(i);
    };
    return os;
}

void GameState::print_debug_board() const {
    cout << *this;
    cout << "Pieces map: " << endl;
    cout << this->pieces_map << endl;
    cout << "Debug info: " << endl;
    this->print_undo_stack();
}

void GameState::print_undo_stack() const {
    cout << "Undo stack:" << endl;
    for (const UndoNode& undo : this->undo_stack) {
	cout << undo << endl;
    }
}

ostream& operator<<(ostream& os, const GameState& state) {
    os << "Current Turn: " << player_repr(state.current_turn()) << endl;
    os << state.pieces_array << endl;
    return os;
}

ostream& operator<<(ostream& os, const UndoNode& undo) {
    return os << "Undo(" << undo.move << "," << undo.former_occupant << ")";
}

void GameState::rollback() {
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

Player GameState::current_turn() const {
    return this->turn;
}

void GameState::current_turn(Player player) {
    this->turn = player;
}
