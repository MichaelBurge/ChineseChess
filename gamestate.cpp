#include "gamestate.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

extern Player next_player(Player);

GameState::GameState(Player _current_turn) {
  this->pieces = map<Position, Piece>();
  this->current_turn = _current_turn;
  this->undo_stack = list<UndoNode>();
}    

UndoNode::UndoNode(const GameState& state, const Move& move) : move(move) {
    this->former_occupant = state.get_piece(move.to);
}

optional<Piece> GameState::get_piece(const Position& position) const {
    auto p = this->pieces.find(position);
    return (p == this->pieces.end())
	? optional<Piece>()
	: (*p).second;
}

void GameState::insert_piece(const Position& position, const Piece& piece) {
    this->pieces.insert(
        pair<Position, Piece>(
            position, piece));
}

template<typename T> T GameState::peek_move(Move move, const function<T(const GameState &)>& action) const {
    auto scratch = GameState(*this);
    scratch.apply_move(move);
    return action(scratch);
}

template int GameState::peek_move<int>(Move, const function<int(const GameState &)>&) const;
template bool GameState::peek_move<bool>(Move, const function<bool(const GameState &)>&) const;

template<> void GameState::peek_move(Move move, const function<void(const GameState &)>& action) const {
    auto& scratch = const_cast<GameState&>(*this);
    scratch.undo_stack.push_back(UndoNode(scratch, move));
    scratch.apply_move(move);
    action(scratch);
    scratch.rollback();
}

void GameState::apply_move(const Move& move) {
    auto i = this->pieces.find(move.from);
    if (i == this->pieces.end())
        throw logic_error("No piece in the 'from' coordinate of this move");
    auto piece =  (*i).second;
    this->pieces.erase(i);
    this->pieces[move.to] = piece;
    this->current_turn = next_player(this->current_turn);
}

void GameState::for_each_piece(function<void(Position, Piece)> action) const {
    for_each(this->pieces.begin(), this->pieces.end(), [&] (const pair<Position, Piece> pair) {
	action(pair.first, pair.second);
    });
}

vector<Position> GameState::filter_pieces(function<bool(Position, Piece)> pred) const {
    auto matches = vector<Position>();
    this->for_each_piece([&] (Position position, Piece piece) {
        if (pred(position, piece))
            matches.push_back(position);
    });
    return matches;
}

vector<Position> GameState::filter_pieces_by_type(PieceType type) const {
    return this->filter_pieces([&] (const Position& position, const Piece& piece) {
	return piece.piece_type == type;
    });
}

Piece::Piece(PieceType piece_type, Player owner) : piece_type(piece_type), owner(owner) { }

multi_array<Piece, 2> GameState::to_board() const {
    multi_array<Piece, 2> ret(extents[10][9]);
    fill(ret.data(), ret.data() + ret.num_elements(), Piece(EMPTY, RED));
    this->for_each_piece([&] (const Position& position, Piece piece) {
        cout << position << " " << character_for_piece(piece) << endl;
        ret[position.rank-1][position.file-1] = piece;
    });
    return ret;
}

Piece::Piece() { }

char GameState::character_for_piece(Piece piece) {
    auto c = '\a';
    switch (piece.piece_type) {
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
        throw logic_error("Unknown piece: " + lexical_cast<string>(piece.piece_type));
    }
    if (piece.owner == RED)
        c = toupper(c);
    return c;
}

string player_repr(Player player) {
    if (player == RED)
        return "Red";
    else
        return "Black";
}

void GameState::print_board() const {
    multi_array<Piece, 2> board = this->to_board();

    auto draw_river = [] () {
	for (int i = 0; i != 8; i++)
	    cout << "~~";
        cout << "~" << endl;
    };

    auto draw_rank = [&] (int rank) {
	for (int i = 0; i != 8; i++)
            cout << character_for_piece(board[rank][i]) << ' ';
        cout << endl;
    };

    cout << "Current Turn: " << player_repr(this->current_turn) << endl;
    for (int i = 9; i --> 0;) {
        if (i == 4)
            draw_river();
        draw_rank(i);
    };
}

void GameState::print_debug_board() const {
    this->print_board();
    cout << "Debug info: " << endl;
    this->print_undo_stack();
}

void GameState::print_undo_stack() const {
    cout << "Undo stack:" << endl;
    for (const UndoNode& undo : this->undo_stack) {
	cout << undo << endl;
    }
}

ostream& operator<<(ostream& os, const UndoNode& undo) {
    return os << "Undo(" << undo.move << "," << undo.former_occupant << ")";
}

void GameState::remove_piece(const Position& position) {
    auto piece_iter = this->pieces.find(position);
    if (piece_iter == this->pieces.end())
	throw logic_error("Tried to remove a nonexistent piece");
    this->pieces.erase(piece_iter);
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
    this->insert_piece(move.from, *piece);
    if (!!node.former_occupant)
	this->insert_piece(move.to, *node.former_occupant);
    else
	this->remove_piece(move.to);
}
