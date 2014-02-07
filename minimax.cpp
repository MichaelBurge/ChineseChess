#include "minimax.hpp"

const int lowest = numeric_limits<int>::min() + 1;
const int highest = numeric_limits<int>::max();

int negamax_basic(const GameState& state, int depth, function<int(const GameState&)> valuation) {
    if (depth == 0)
	return valuation(state);
    int best_value = lowest;
    if (available_moves(state).empty())
	cout << "empty!" << endl;
    for (const Move& move : available_moves(state)) {
	peek_move<void>(state, move, false, [&] (const GameState& newState) -> void {
	    auto val = -negamax_basic(newState, depth - 1, valuation);
	    best_value = max(best_value, val);
        });
    }
    if (best_value == lowest)
	cout << "lowest!" << endl;
    return best_value;
}

int negamax(const GameState& state, int depth, int& node_count, function<int(const GameState&)> valuation) {
    //    return negamax_with_pruning(state, depth, node_count, lowest, highest, valuation);
    auto value = -negamax_basic(state, depth, valuation);
    return value;
}

void map_negamax(const GameState& state, int depth, int node_count, function<int(const GameState&)> valuation, function<void(const Move&, int value)> action) {
    auto moves = available_moves(state);
    if (moves.empty())
        throw logic_error("No move exists");
    for (const Move& move : moves) {
        peek_move<void>(state, move, false, [&] (const GameState& newState) {
            auto value = negamax(newState, depth, node_count, valuation);
            action(move, value);
        });
    }
}

Move best_move(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> valuation) {
     auto best_value = lowest;
     Move best_move;
     map_negamax(state, depth, max_nodes, valuation, [&] (const Move& move, int value) {
         if (value > best_value) {
             best_move = move;
             best_value = value;
         }
     });

     return best_move;

}

vector<pair<Move, int> > move_scores(const GameState& state, function<int(const GameState&)> valuation) {
    auto moves = available_moves(state);
    auto ret = vector<pair<Move, int> >();
    for (const Move& move : moves) {
	peek_move<void>(state, move, false, [&] (const GameState& newState) -> void {
	    auto value = valuation(newState);
	    ret.push_back(pair<Move, int>(move, value));
	});
    }
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
