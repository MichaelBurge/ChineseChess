#pragma once

#include <boost/multi_array.hpp>
#include <boost/optional/optional.hpp>
#include <vector>
#include "position.hpp"
#include "direction.hpp"
#include "gamestate.hpp"
#include "move.hpp"

using namespace std;
using namespace boost;

// Basic game rules
extern GameState    new_game();
extern vector<Move> available_moves(const GameState &);
extern vector<Move> available_moves_without_check(const GameState &);
extern bool         is_capture(const GameState&, const Move&);
extern bool         is_legal_move(const GameState &, const Move&, bool allow_check = false);
extern optional<Player> winner(const GameState&);
extern bool         is_king_in_check(const GameState& state, Player);
extern Player       next_player(Player);

// Debugging aids
extern bool violates_flying_kings_rule(const GameState&);
extern bool violates_kings_in_castle_rule(const GameState& state);
extern bool violates_can_only_capture_enemy_pieces_rule(const GameState& state, const Move& move);
extern bool is_invalid_state(const GameState &);
extern bool is_position_valid(const Position &);
extern bool results_in_check(const GameState&, const Move&);

// Querying state for useful information
extern vector<Move> filter_available_moves(const GameState& state, function<bool(const Move&)> pred);
extern vector<Move> filter_available_captures(const GameState& state, function<bool(const Move&)> pred);
extern vector<Move> captures_for_position(const GameState& state, const Position& position);
extern vector<Move> available_moves_from(const GameState& state, const Position& position);
extern int          num_available_moves(const GameState&);
extern int          num_available_captures(const GameState&);

extern void print_available_moves(const GameState &);
