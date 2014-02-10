#pragma once

#include "move.hpp"
#include <boost/multi_array.hpp>
#include <functional>
#include <boost/optional/optional.hpp>
#include <list>
#include <map>
#include <stack>
using namespace std;
using namespace boost;

struct GameState;

enum PieceType { EMPTY, GENERAL, ADVISOR, ELEPHANT, HORSE, CHARIOT, CANNON, SOLDIER };
enum Player { RED, BLACK };

struct Piece {
    PieceType piece_type;
    Player owner;
    Piece();
    Piece(PieceType, Player);
};

struct UndoNode {
    Move move;
    optional<Piece> former_occupant;
    UndoNode(const GameState&, const Move&);
};

struct GameState {
    Player current_turn;
    GameState(Player _current_turn);
    // Mutating methods
    void insert_piece(const Position&, const Piece&);
    void apply_move(const Move&);

    // Const methods
    template<typename T> T peek_move(Move, const function<T(const GameState &)>& action) const;

    optional<Piece> get_piece(const Position&) const;
    void for_each_piece(function<void(Position, Piece)> action) const;
    vector<Position> filter_pieces(function<bool(Position, Piece)> pred) const;
    vector<Position> filter_pieces_by_type(PieceType type) const;
    multi_array<Piece, 2> to_board() const;
    void print_board() const;
private:
    char character_for_piece(Piece);
    mutable map<Position, Piece> pieces;
    mutable stack<UndoNode, list<UndoNode> > undo_stack;
};
