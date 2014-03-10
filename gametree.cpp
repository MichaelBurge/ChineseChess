#include "gametree.hpp"
#include <iostream>
using namespace std;

const int lowest = numeric_limits<int>::min() + 1;

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

int negamax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation) {
    typedef MinimaxSearch<int> SearchProcedure;
    TranspositionTable<int> transposition_table;
    SearchProcedure search(state, depth, valuation);
    return generalized_tree_fold<int, SearchProcedure>(
	transposition_table,
	search
    );
}

void map_negamax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation, function<void(const Move&, int value)> action) {
    auto moves = StandardRulesEngine::available_moves(state);
    if (moves.empty())
        throw logic_error("No move exists");
    for (const Move& move : moves) {
        state.peek_move(move, [&] (const StandardGameState& newState) {
            auto value = -negamax(newState, depth, valuation);
	    action(move, value);
	});
    }
}

Move best_move(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation) {
    auto best_value = lowest;
    Move best_move = Move(Position(-1, -1), Position(-1, -1));
    auto found = false;
    map_negamax(state, depth, valuation, [&] (const Move& move, int value) {
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

vector<pair<Move, int> > move_scores(const StandardGameState& state, function<int(const StandardGameState&)> valuation) {
    auto moves = StandardRulesEngine::available_moves(state);
    auto ret = vector<pair<Move, int> >();
    for (const Move& move : moves) {
	state.peek_move(move, [&] (const StandardGameState& newState) -> void {
	    auto value = -valuation(newState);
	    ret.push_back(pair<Move, int>(move, value));
	});
    }
    return ret;
}

vector<pair<Move, int> > move_scores_minimax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation) {
    auto ret = vector<pair<Move, int> >();
    map_negamax(state, depth, valuation, [&] (const Move& move, int value) {
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

vector<Move> best_move_sequence(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation) {
    auto state_accum = state;
    auto ret = vector<Move>();
    for (;depth; depth--) {
	auto best = best_move(state_accum, depth, valuation);
	state_accum.apply_move(best);
	ret.push_back(best);
    }
    return ret;
}

void print_move_sequence(const vector<Move>& moves) {
    int turn = 1;
    for (const Move& move : moves) {
	cout << turn++ << ". " << move << endl;
    }
}

void print_analysis(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation) {
    auto moves = best_move_sequence(state, depth, valuation);
    auto state_accum = state;
    int turn = 1;
    int color = 1;
    for (const Move& move : moves) {
	state_accum.apply_move(move);
	color *= -1;
	int score = color * negamax(state_accum, depth, valuation);
	cout << turn++ << ". " << move << " - Score: " << score << endl;
    }
}
