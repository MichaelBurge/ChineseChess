#pragma once

#include "../move.hpp"
#include "../piece.hpp"
#include "../player.hpp"
#include "../position.hpp"
#include "../uint128_t.hpp"
#include "bitboard.hpp"
#include <functional>
#include <list>
using namespace std;

typedef uint128_t bitboard;

struct Cell {
    Cell(const Position& position, const Piece& piece) : position(position), piece(piece) { }
    Position position;
    Piece piece;
};

struct BitboardGameState {
    BitboardGameState(Player player) : _current_turn(player) {};
    // Fundamental data (Consistent during state-manipulating methods)
    bitboard all_pieces;
    bitboard red_pieces; 
    bitboard black_pieces;
    bitboard generals;
    bitboard advisors;
    bitboard elephants;
    bitboard horses;
    bitboard chariots;
    bitboard cannons;
    bitboard soldiers;

    // Computed data (Other people will compute and cache these using the above information)
    mutable bool is_cache_valid;
    mutable bitboard moves;

    // Standard GameState methods
    Piece get_piece(const Position&) const;
    void insert_piece(const Position&, const Piece&);
    void remove_piece(const Position&);
    void apply_move(const Move&);
    void current_turn(Player);
    Player current_turn() const;
    void switch_turn();
    void peek_move(const Move&, const function<void(const BitboardGameState&)>& action) const;
    void print_debug_board() const;
    void for_each_piece(function<void(Position, Piece)> action) const;
    void for_each_red_piece(function<void(Position, Piece)> action) const;
    void for_each_black_piece(function<void(Position, Piece)> action) const;
    inline void for_each_player_piece(Player player, function<void(Position, Piece)> action) const {
	if (player == RED)
	    for_each_red_piece(action);
	else
	    for_each_black_piece(action);
    }
    bool check_internal_consistency() const;
private:
    void ensure_moves_cached();
    void clear_cached_data();
 Player _current_turn;
};
