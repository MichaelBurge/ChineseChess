#include "minimax.hpp"
#include "rules-engines/reference.hpp"

const int lowest = numeric_limits<int>::min() + 1;
const int highest = numeric_limits<int>::max();

int negamax_basic(const GameState& state, int depth, function<int(const GameState&)> valuation) {
    int best_value = lowest;
    auto moves = rules->available_moves(state);
    if (moves.empty())
	return best_value;
    if (depth == 0)
	return valuation(state);
    for (const Move& move : rules->available_moves(state)) {
	state.peek_move<void>(move, [&] (const GameState& newState) -> void {
	    auto val = -negamax_basic(newState, depth - 1, valuation);
	    best_value = max(best_value, val);
        });
    }
    return best_value;
}

void reorder_moves(const GameState& state, vector<Move>& available_moves, function<int(const GameState&, const Move&)> valuation) {
}

int negamax_with_pruning(const GameState& state, int depth, int alpha, int beta, function<int(const GameState&)> valuation) {
    int best_value = lowest;
    auto moves = rules->available_moves(state);
    if (moves.empty())
	return best_value;
    if (depth == 0)
	return valuation(state);
    auto available_moves = rules->available_moves(state);
    //    reorder_moves(
    for (const Move& move : available_moves) {
	state.peek_move<void>(move, [&] (const GameState& newState) -> void {
            auto val = -negamax_with_pruning(newState, depth - 1, -beta, -alpha, valuation);
	    best_value = max(best_value, val);
	    alpha = max(alpha, val);
	});
	if (alpha >= beta)
	    break;
    }
    return best_value;
}

int negamax(const GameState& state, int depth, int& node_count, function<int(const GameState&)> valuation) {
    //    return negamax_with_pruning(state, depth, node_count, lowest, highest, valuation);
    // auto value = negamax_basic(state, depth, valuation);
    auto value = negamax_with_pruning(state, depth, lowest, highest, valuation);
    return value;
}

void map_negamax(const GameState& state, int depth, int node_count, function<int(const GameState&)> valuation, function<void(const Move&, int value)> action) {
    auto moves = rules->available_moves(state);
    if (moves.empty())
        throw logic_error("No move exists");
    for (const Move& move : moves) {
        state.peek_move<void>(move, [&] (const GameState& newState) {
            auto value = -negamax(newState, depth, node_count, valuation);
            action(move, value);
        });
    }
}

Move best_move(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> valuation) {
     auto best_value = lowest;
     Move best_move = Move(Position(-1, -1), Position(-1, -1));
     auto found = false;
     map_negamax(state, depth, max_nodes, valuation, [&] (const Move& move, int value) {
         found = true;
         if (value >= best_value) {
             best_move = move;
             best_value = value;
         }
     });
     if (!found)
	 throw logic_error("No moves available");
     return best_move;

}

vector<pair<Move, int> > move_scores(const GameState& state, function<int(const GameState&)> valuation) {
    auto moves = rules->available_moves(state);
    auto ret = vector<pair<Move, int> >();
    for (const Move& move : moves) {
	state.peek_move<void>(move, [&] (const GameState& newState) -> void {
	    auto value = -valuation(newState);
	    ret.push_back(pair<Move, int>(move, value));
	});
    }
    return ret;
}

vector<pair<Move, int> > move_scores_minimax(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> valuation) {
    auto ret = vector<pair<Move, int> >();
    map_negamax(state, depth, max_nodes, valuation, [&] (const Move& move, int value) {
        ret.push_back(pair<Move, int>(move, value));
    });
    return ret;
}

void print_move_scores(const vector<pair<Move, int> >& scores) {
    auto sorted_scores = scores;
    sort(sorted_scores.begin(), sorted_scores.end(), [] (const pair<Move, int>& a, const pair<Move, int>& b) {
        return a.second < b.second;
    });

    if (sorted_scores.empty())
        cout << "No moves available!" << endl;

    for_each(sorted_scores.begin(), sorted_scores.end(), [] (const pair<Move, int>& score) {
        cout << score.first << ": " << score.second << endl;
    });
}

vector<Move> best_move_sequence(const GameState& state, int depth, function<int(const GameState&)> valuation) {
    auto state_accum = state;
    auto ret = vector<Move>();
    for (;depth; depth--) {
	auto best = best_move(state_accum, depth, 1000000, valuation);
	cout << "Best move: " << best << endl;
	state_accum.apply_move(best);
	ret.push_back(best);
    }
    return ret;
}
