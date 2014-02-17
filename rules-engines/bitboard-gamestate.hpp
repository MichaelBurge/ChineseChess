#pragma once

#include "../move.hpp"
#include "../piece.hpp"
#include "../player.hpp"
#include "../position.hpp"
#include "../uint128_t.hpp"
#include <functional>
#include <vector>
using namespace std;

typedef uint128_t bitboard;

struct Cell {
    Position position;
    Piece piece;
};

struct BitboardGameState {
    BitboardGameState(Player player) : _current_turn(player) {};
    // Fundamental data
    vector<Cell> pieces;
    bitboard red_generals;
    bitboard red_advisors;
    bitboard red_elephants;
    bitboard red_horses;
    bitboard red_chariots;
    bitboard red_cannons;
    bitboard red_soldiers;
    bitboard black_generals;
    bitboard black_advisors;
    bitboard black_elephants;
    bitboard black_horses;
    bitboard black_chariots;
    bitboard black_cannons;
    bitboard black_soldiers;

    // Computed data
    bitboard all_pieces;
    bitboard red_pieces;
    bitboard black_pieces;
    bitboard moves;

    // Standard GameState methods
    Piece get_piece(const Position&) const;
    void insert_piece(const Position&, const Piece&);
    void remove_piece(const Position&);
    void apply_move(const Move&);
    void current_turn(Player);
    Player current_turn() const;
    void peek_move(const Move&, const function<void(const BitboardGameState&)>& action) const;
    void print_debug_board() const;
    void for_each_piece(function<void(Position, Piece)> action) const;
private:
    Player _current_turn;
};

extern ostream& operator<<(ostream& os, const BitboardGameState& state);
