#include "gamestate.hpp"
#include <algorithm>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

extern Player next_player(Player);

GameState::GameState(Player _current_turn) {
  this->pieces = map<Position, Piece>();
  this->current_turn = _current_turn;
  this->undo_stack = stack<UndoNode, list<UndoNode> >();
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
    auto scratch = GameState(*this);
    scratch.apply_move(move);
    action(scratch);
}

void GameState::apply_move(const Move& move) {
    auto i = this->pieces.find(move.from);
    if (i == this->pieces.end())
        throw logic_error("No piece in the 'from' coordinate of this move");

    this->pieces[move.to] = (*i).second;
    this->pieces.erase(i);
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
