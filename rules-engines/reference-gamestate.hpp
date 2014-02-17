#pragma once

#include "../gamestate.hpp"
#include "../move.hpp"
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/multi_array.hpp>
#include <boost/optional/optional.hpp>
#include <functional>
#include <list>
#include <map>

using namespace std;
using namespace boost;
using namespace boost::bimaps;

struct ReferenceGameState;

struct UndoNode {
    Move move;
    optional<Piece> former_occupant;
    UndoNode(const ReferenceGameState&, const Move&);
};
ostream& operator<<(ostream& os, const UndoNode&);

struct ReferenceGameStateStorageMethod {
    virtual ~ReferenceGameStateStorageMethod() {};
    virtual Piece get_piece(const Position& position) const = 0;
    virtual void remove_piece(const Position&) = 0;
    virtual void for_each_piece(function<void(Position, Piece)> action) const = 0;
};

struct ReferenceGameStateArrayStorage : ReferenceGameStateStorageMethod {
    ReferenceGameStateArrayStorage();
    virtual Piece get_piece(const Position& position) const;
    virtual void insert_piece(const Position&, const Piece&);
    virtual void remove_piece(const Position&);
    virtual void for_each_piece(function<void(Position, Piece)> action) const;
private:
    multi_array<Piece, 2> pieces;
};

struct ReferenceGameStateDictionaryStorage : ReferenceGameStateStorageMethod {
    ReferenceGameStateDictionaryStorage();
    virtual Piece get_piece(const Position& position) const;
    virtual void insert_piece(const Position&, const Piece&);
    virtual void remove_piece(const Position&);
    virtual void for_each_piece(function<void(Position, Piece)> action) const;
    int size() const;
private:
    bimap<Position, multiset_of<Piece> > pieces;
};
ostream& operator<<(ostream& os, const ReferenceGameStateDictionaryStorage&);

struct ReferenceGameState : ReferenceGameStateStorageMethod {
    ReferenceGameState(Player _current_turn);

    virtual Piece get_piece(const Position&) const;
    virtual void insert_piece(const Position&, const Piece&);
    virtual void remove_piece(const Position&);
    virtual void for_each_piece(function<void(Position, Piece)> action) const;

    virtual void apply_move(const Move&);
    void current_turn(Player);
    void switch_turn();

    void peek_move(const Move&, const function<void(const ReferenceGameState&)>& action) const;
    Player current_turn() const;
    vector<Position> filter_pieces(function<bool(Position, Piece)> pred) const;
    void print_debug_board() const;
    void print_undo_stack() const;
    optional<Position> get_king_position(Player) const;
private:
    void commit();
    void rollback();

    mutable optional<Position> red_king;
    mutable optional<Position> black_king;
    mutable Player turn;
    mutable ReferenceGameStateArrayStorage pieces_array;
    mutable ReferenceGameStateDictionaryStorage pieces_map;
    mutable list<UndoNode> undo_stack;
    friend ostream& operator<<(ostream& os, const ReferenceGameState&);
};

extern ostream& operator<<(ostream& os, const ReferenceGameState&);
extern char character_for_piece(Piece piece);
