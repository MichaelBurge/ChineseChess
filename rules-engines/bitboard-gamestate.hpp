#pragma once

typedef uint128_t bitboard;
typedef uint8_t Position;

struct Cell {
    Position position;
    Piece piece;
};

struct BitboardGameState {
    // Fundamental data
    bool current_turn;
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
    bitboard red_pieces;
    bitboard black_pieces;
    bitboard moves;

    // Standard GameState methods
    Piece get_piece(const Position&) const;
    void insert_piece(const Position&, const Piece&);
    void remove_piece(const Position&);
    void apply_move(const Move&);
    void current_turn(Player);
    Player current_turn();
    void peek_move(const Move&, const function<void(const ReferencGameState&)>& action) const;
    void print_debug_board() const;
};
