#pragma once

#include <vector>
#include "position.hpp"
#include "direction.hpp"
#include "gamestate.hpp"
#include "move.hpp"

template<class GameState, class Implementation>
struct RulesEngine {
    // Core rules
    static vector<Move> available_moves(const GameState & state)
    { return Implementation::available_moves(state.implementation); }

    static vector<Move> available_moves_without_check(const GameState & state)
    { return Implementation::available_moves_without_check(state.implementation); }

    static bool is_capture(const GameState& state, const Move& move)
    { return Implementation::is_capture(state.implementation, move); }

    static bool is_legal_move(const GameState & state, const Move& move, bool allow_check = false)
    { return Implementation::is_legal_move(state.implementation, move, allow_check); }

    static bool is_winner(const GameState& state)
    { return Implementation::is_winner(state.implementation); }

    static Player winner(const GameState& state)
    { return Implementation::winner(state.implementation); }

    static bool is_king_in_check(const GameState& state, Player player)
    { return Implementation::is_king_in_check(state.implementation, player); }

    // Derived rules
    static bool results_in_check(const GameState& state, const Move& move)
    { return Implementation::results_in_check(state.implementation, move); }

    static vector<Move> filter_available_moves(const GameState& state, function<bool(const Move&)> pred)
    { return Implementation::filter_available_moves(state.implementation, pred); }

    static vector<Move> captures_for_position(const GameState& state, const Position& position)
    { return Implementation::captures_for_position(state.implementation, position); }

    static vector<Move> available_moves_from(const GameState& state, const Position& position)
    { return Implementation::available_moves_from(state.implementation, position); }

    static int num_available_moves(const GameState& state)
    { return Implementation::num_available_moves(state.implementation); }

    static int num_available_captures(const GameState& state)
    { return Implementation::num_available_captures(state.implementation); }

};
