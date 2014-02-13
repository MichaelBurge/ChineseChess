#include "rules-engine.hpp"
#include "rules-engines/reference.hpp"

RulesEngine *rules = &THE_REFERENCE_RULES;

vector<Move> RulesEngine::filter_available_moves(const GameState& state, function<bool(const Move&)> pred) {
    auto moves = this->available_moves_without_check(state);
    auto matches = vector<Move>();
    for_each(moves.begin(), moves.end(), [&] (const Move& move) {
        if (pred(move))
            matches.push_back(move);
    });
    return matches;
}

vector<Move> RulesEngine::captures_for_position(const GameState& state, const Position& position) {
    return this->filter_available_moves(state, [&] (const Move& move) {
        return move.to == position;
    });

}

vector<Move> RulesEngine::available_moves_from(const GameState& state, const Position& position) {
    return filter_available_moves(state, [&] (const Move& move) {
        return move.from == position;
    });
}

int RulesEngine::num_available_moves(const GameState& state) {
    return available_moves(state).size();
}

int RulesEngine::num_available_captures(const GameState& state) {
    return this->filter_available_moves(state, [&] (const Move& move) {
	return this->is_capture(state, move);
    }).size();
}

bool RulesEngine::results_in_check(const GameState& state, const Move& move) {
    return state.peek_move<bool>(move, [&] (const GameState& new_state) {
	    return this->is_king_in_check(new_state, next_player(new_state.current_turn()));
    });
};
