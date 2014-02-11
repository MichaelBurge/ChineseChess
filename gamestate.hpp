#pragma once

#include "move.hpp"
#include <boost/multi_array.hpp>
#include <functional>
#include <boost/optional/optional.hpp>
#include <list>
#include <map>
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
ostream& operator<<(ostream& os, const UndoNode&);

struct GameStateStorageMethod {
    virtual ~GameStateStorageMethod() {};
    virtual optional<Piece> get_piece(const Position& position) const = 0;
    virtual void insert_piece(const Position&, const Piece&) = 0;
    virtual void remove_piece(const Position&) = 0;
    virtual void for_each_piece(function<void(Position, Piece)> action) const = 0;
};

struct GameStateArrayStorage : GameStateStorageMethod {
    GameStateArrayStorage();
    virtual optional<Piece> get_piece(const Position& position) const;
    virtual void insert_piece(const Position&, const Piece&);
    virtual void remove_piece(const Position&);
    virtual void for_each_piece(function<void(Position, Piece)> action) const;

    Piece get_piece_direct(const Position& position) const;
private:
    multi_array<Piece, 2> pieces;
};
ostream& operator<<(ostream& os, const GameStateArrayStorage&);

struct GameStateDictionaryStorage : GameStateStorageMethod {
    GameStateDictionaryStorage();
    virtual optional<Piece> get_piece(const Position& position) const;
    virtual void insert_piece(const Position&, const Piece&);
    virtual void remove_piece(const Position&);
    virtual void for_each_piece(function<void(Position, Piece)> action) const;
private:
    map<Position, Piece> pieces;
};
ostream& operator<<(ostream& os, const GameStateDictionaryStorage&);

struct GameState : GameStateStorageMethod {
    GameState(Player _current_turn);

    virtual optional<Piece> get_piece(const Position&) const;
    virtual void insert_piece(const Position&, const Piece&);
    virtual void remove_piece(const Position&);
    virtual void for_each_piece(function<void(Position, Piece)> action) const;

    virtual void apply_move(const Move&);
    void current_turn(Player);
    void switch_turn();

    template<typename T> T peek_move(Move, const function<T(const GameState &)>& action) const;
    Player current_turn() const;
    vector<Position> filter_pieces(function<bool(Position, Piece)> pred) const;
    vector<Position> filter_pieces_by_type(PieceType type) const;
    void print_board() const;
    void print_debug_board() const;
    void print_undo_stack() const;
private:
    void commit();
    void rollback();
    char character_for_piece(Piece);

    mutable Player turn;
    mutable GameStateArrayStorage pieces_array;
    mutable GameStateDictionaryStorage pieces_map;
    mutable list<UndoNode> undo_stack;
    friend ostream& operator<<(ostream& os, const GameState&);
};

extern ostream& operator<<(ostream& os, const GameState&);
extern char character_for_piece(Piece piece);
