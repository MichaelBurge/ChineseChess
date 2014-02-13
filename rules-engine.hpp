#pragma once
#include <boost/optional/optional.hpp>
#include <vector>
#include "position.hpp"
#include "direction.hpp"
#include "gamestate.hpp"
#include "move.hpp"

class RulesEngine {
public:
    // Core rules
    virtual GameState    new_game() = 0;
    virtual vector<Move> available_moves(const GameState &) = 0;
    virtual vector<Move> available_moves_without_check(const GameState &) = 0;
    virtual bool         is_capture(const GameState&, const Move&) = 0;
    virtual bool         is_legal_move(const GameState &, const Move&, bool allow_check = false) = 0;
    virtual optional<Player> winner(const GameState&) = 0;
    virtual bool         is_king_in_check(const GameState& state, Player) = 0;

    // Derived rules
    virtual vector<Move> filter_available_moves(const GameState& state, function<bool(const Move&)> pred);
    virtual vector<Move> captures_for_position(const GameState& state, const Position& position);
    virtual vector<Move> available_moves_from(const GameState& state, const Position& position);
    virtual int          num_available_moves(const GameState&);
    virtual int          num_available_captures(const GameState&);
    bool                 results_in_check(const GameState& state, const Move& move);
};
