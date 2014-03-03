#include "gametree.hpp"

void enumerate_tree(const StandardGameState& initial, int depth, function<void(const StandardGameState&)> action) {
    if (depth == 0) {
	action(initial);
	return;
    }
    for (const Move& move : StandardRulesEngine::available_moves(initial)) {
	initial.peek_move(move, [&] (const StandardGameState& new_state) {
	    enumerate_tree(new_state, depth - 1, action);
	});
    }
}

int negamax_with_pruning(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation) {
    typedef MinimaxSearch<int> SearchProcedure;
    vector<optional<int> > transposition_table(TRANSPOSITION_TABLE_SIZE, optional<int>());
    SearchProcedure search(valuation);
    return generalized_tree_fold<int, SearchProcedure>(state, depth, transposition_table, search);
}
