#pragma once

struct LookupTable {
    bitboard boards[90];
};

struct DirectionalLookupTable {
    LookupTable tables[90];
};

namespace bitboard_implementation {
    extern vector<Move> _available_moves(const BitboardGameState& state);
    extern vector<Move> _available_moves_without_check(const BitboardGameState& state);
    extern bool         _is_capture(const BitboardGameState&, const Move&);
    extern bool         _is_legal_move(const BitboardGameState &, const Move&, bool allow_check = false);
    extern Player       _winner(const BitboardGameState& state);
    extern bool         _is_winner(const BitboardGameState& state);
    extern bool         _is_king_in_check(const BitboardGameState& state, Player);
    extern vector<Move> _filter_available_moves(const BitboardGameState& state, function<bool(const Move&)> pred);
    extern vector<Move> _captures_for_position(const BitboardGameState& state, const Position& position);
    extern vector<Move> _available_moves_from(const BitboardGameState& state, const Position& position);
    extern int          _num_available_moves(const BitboardGameState& state);
    extern int          _num_available_captures(const BitboardGameState& state);
    extern bool         _results_in_check(const BitboardGameState& state, const Move& move);

    // Auxillary functions
    bitboard generate_entire_board();
    bitboard generate_castle_area();
    bitboard generate_red_side();
    LookupTable generate_soldier_moves_lookup_table(bool);
    LookupTable generate_general_moves_lookup_table();
    LookupTable generate_advisor_moves_lookup_table();
    DirectionalLookupTable generate_horse_moves_lookup_table();
    DirectionalLookupTable generate_chariot_ideal_moves_table();
    bitboard moves_for_red_soldier(Position position);
    bitboard moves_for_black_soldier(Position position);
    bitboard moves_for_elephant(Position position);
    bitboard moves_for_general(Position position);
    bitboard moves_for_advisor(Position position);
    bitboard moves_for_horse(const BitboardGameState& state, Position position);
    bitboard moves_for_chariot(const BitboardGameState& state, Position position);
    bitboard moves_for_cannon(const BitboardGameState& state, Position position);
    bitboard moves_for_piece(const BitboardGameState& state, Position position, Piece piece);
    bitboard compute_reachable_positions(const BitboardGameState& state);
    void insert_vectorized_moves(const bitboard& board, const Position& root, vector<Move>& moves);
    void ensure_moves_cached(const BitboardGameState& state);
};

class BitboardRules {
public:
    // Core rules
    static vector<Move> available_moves(const BitboardGameState & state)
    { return bitboard_implementation::_available_moves(state); }

    static vector<Move> available_moves_without_check(const BitboardGameState & state)
    { return bitboard_implementation::_available_moves_without_check(state); }

    static bool         is_capture(const BitboardGameState& state, const Move& move)
    { return bitboard_implementation::_is_capture(state, move); }

    static bool         is_legal_move(const BitboardGameState & state, const Move& move, bool allow_check = false)
    { return bitboard_implementation::_is_legal_move(state, move, allow_check); }

    static bool is_winner(const BitboardGameState& state)
    { return bitboard_implementation::_is_winner(state); }

    static Player winner(const BitboardGameState& state)
    { return bitboard_implementation::_winner(state); }

    static bool         is_king_in_check(const BitboardGameState& state, Player player)
    { return bitboard_implementation::_is_king_in_check(state, player); }

    static bool results_in_check(const BitboardGameState& state, const Move& move)
    { return bitboard_implementation::_results_in_check(state, move); }

    static vector<Move> filter_available_moves(const BitboardGameState& state, function<bool(const Move&)> action)
    { return bitboard_implementation::_filter_available_moves(state, action); }

    static vector<Move> captures_for_position(const BitboardGameState& state, const Position& position)
    { return bitboard_implementation::_captures_for_position(state, position); }

    static vector<Move> available_moves_from(const BitboardGameState& state, const Position& position)
    { return bitboard_implementation::_available_moves_from(state, position); }

    static int num_available_moves(const BitboardGameState& state)
    { return bitboard_implementation::_num_available_moves(state); }

    static int num_available_captures(const BitboardGameState& state)
    { return bitboard_implementation::_num_available_captures(state); }
};
