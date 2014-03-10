#pragma once

#include "../rules-engine.hpp"
#include "reference-gamestate.hpp"

namespace implementation {
    extern vector<Move> _available_moves(const ReferenceGameState &);
    extern vector<Move> _available_moves_without_check(const ReferenceGameState &);
    extern bool         _is_capture(const ReferenceGameState&, const Move&);
    extern bool         _is_legal_move(const ReferenceGameState &, const Move&, bool allow_check = false);
    extern Player       _winner(const ReferenceGameState& state);
    extern bool         _is_winner(const ReferenceGameState& state);
    extern bool         _is_king_in_check(const ReferenceGameState& state, Player);
    extern vector<Move> _filter_available_moves(const ReferenceGameState& state, function<bool(const Move&)> pred);
    extern vector<Move> _captures_for_position(const ReferenceGameState& state, const Position& position);
    extern vector<Move> _available_moves_from(const ReferenceGameState& state, const Position& position);
    extern int          _num_available_moves(const ReferenceGameState& state);
    extern int          _num_available_captures(const ReferenceGameState& state);
    extern bool         _results_in_check(const ReferenceGameState& state, const Move& move);

    // Auxillary function
    extern void insert_available_moves_for_general(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_advisor(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_horse(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_elephant(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_chariot(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_cannon(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_soldier(const ReferenceGameState&, Position, Player, vector<Move>&);
    extern void insert_available_moves_for_piece(const ReferenceGameState&, Position, Piece piece, vector<Move>&);
    extern optional<Position> shoot_ray_in_direction_until_collision(const ReferenceGameState&, const Position&, Direction);
    extern optional<Piece> shoot_ray_in_direction_until_piece(const ReferenceGameState& state, const Position& center, Direction direction);
    extern bool violates_flying_kings_rule(const ReferenceGameState& state);
    extern bool violates_pieces_stuck_in_castle_rule(const ReferenceGameState& state);
    extern bool violates_can_only_capture_enemy_pieces_rule(const ReferenceGameState& state, const Move& move);
    extern bool is_invalid_state(const ReferenceGameState& state);
    extern bool is_position_in_castle(const Position& position);
    extern bool is_position_occupied(const ReferenceGameState& state, Position position);
    extern void filter_invalid_moves(const ReferenceGameState& state, vector<Move>& moves);
    extern bool has_crossed_river(const Position& position, Player player);

    extern vector<Move> filter_available_moves(const ReferenceGameState&, function<bool(const Move&)>);
    extern vector<Move> captures_for_position(const ReferenceGameState&, const Position&);
    extern vector<Move> available_moves_from(const ReferenceGameState&, const Position&);
    extern int num_available_moves(const ReferenceGameState&);
    extern int num_available_captures(const ReferenceGameState&);
    extern optional<Player> __winner(const ReferenceGameState&);
}

class ReferenceRules {
public:
    // Core rules
    static vector<Move> available_moves(const ReferenceGameState & state)
    { return implementation::_available_moves(state); }

    static vector<Move> available_moves_without_check(const ReferenceGameState & state)
    { return implementation::_available_moves_without_check(state); }

    static bool         is_capture(const ReferenceGameState& state, const Move& move)
    { return implementation::_is_capture(state, move); }

    static bool         is_legal_move(const ReferenceGameState & state, const Move& move, bool allow_check = false)
    { return implementation::_is_legal_move(state, move, allow_check); }

    static bool is_winner(const ReferenceGameState& state)
    { return implementation::_is_winner(state); }

    static Player winner(const ReferenceGameState& state)
    { return implementation::_winner(state); }

    static bool         is_king_in_check(const ReferenceGameState& state, Player player)
    { return implementation::_is_king_in_check(state, player); }

    static bool results_in_check(const ReferenceGameState& state, const Move& move)
    { return implementation::_results_in_check(state, move); }

    static vector<Move> filter_available_moves(const ReferenceGameState& state, function<bool(const Move&)> action)
    { return implementation::_filter_available_moves(state, action); }

    static vector<Move> captures_for_position(const ReferenceGameState& state, const Position& position)
    { return implementation::_captures_for_position(state, position); }

    static vector<Move> available_moves_from(const ReferenceGameState& state, const Position& position)
    { return implementation::_available_moves_from(state, position); }

    static int num_available_moves(const ReferenceGameState& state)
    { return implementation::_num_available_moves(state); }

    static int num_available_captures(const ReferenceGameState& state)
    { return implementation::_num_available_captures(state); }
};
