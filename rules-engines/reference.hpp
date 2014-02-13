#pragma once

#include "../rules-engine.hpp"

class ReferenceRules : public RulesEngine {
public:
    // Core rules
    virtual GameState    new_game();
    virtual vector<Move> available_moves(const GameState &);
    virtual vector<Move> available_moves_without_check(const GameState &);
    virtual bool         is_capture(const GameState&, const Move&);
    virtual bool         is_legal_move(const GameState &, const Move&, bool allow_check = false);
    virtual optional<Player> winner(const GameState&);
    virtual bool         is_king_in_check(const GameState& state, Player);
private:
    void insert_available_moves_for_general(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_advisor(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_horse(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_elephant(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_chariot(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_cannon(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_soldier(const GameState&, Position, Player, vector<Move>&);
    void insert_available_moves_for_piece(const GameState&, Position, Piece piece, vector<Move>&);
    optional<Position> shoot_ray_in_direction_until_collision(const GameState&, const Position&, Direction);
    optional<Piece> shoot_ray_in_direction_until_piece(const GameState& state, const Position& center, Direction direction);
    bool violates_flying_kings_rule(const GameState& state);
    bool violates_pieces_stuck_in_castle_rule(const GameState& state);
    bool violates_can_only_capture_enemy_pieces_rule(const GameState& state, const Move& move);
    bool is_invalid_state(const GameState& state);
    bool is_position_in_castle(const Position& position);
    bool is_position_occupied(const GameState& state, Position position);
    void filter_invalid_moves(const GameState& state, vector<Move>& moves);
    bool has_crossed_river(const Position& position, Player player);
    void print_available_moves(const GameState& state);
};


extern ReferenceRules THE_REFERENCE_RULES;
