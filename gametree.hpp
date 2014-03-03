#pragma once

#include "configuration.hpp"

const int TRANSPOSITION_TABLE_SIZE = 1000;

extern void enumerate_tree(const StandardGameState&, int, function<void(const StandardGameState&)>);
extern int negamax_with_pruning(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);

template<typename T>
struct MinimaxSearch {
    typedef function<T(const StandardGameState&)> Valuation;
    T best_value;
    T alpha;
    T beta;
    Valuation valuation;
    MinimaxSearch(Valuation valuation) :
	best_value(numeric_limits<T>::min()),
	alpha(numeric_limits<T>::min()),
	beta(numeric_limits<T>::max()),
	valuation(valuation)
        {}
    inline int identity()
    { return 0; }

    inline bool should_cut() {
	return alpha >= beta;
    }

    inline T on_leaf_node(const StandardGameState& state) {
	return valuation(state);
    }

    inline T on_branch_node(const StandardGameState& state, const T& value) {
	alpha = max(alpha, value);
	return value;
    }

    inline void accumulate(const T& value) {
	if (value > best_value)
	    best_value = value;
    }

    inline MinimaxSearch<T> next() {
	MinimaxSearch<T> ret(valuation);
	ret.alpha = -beta;
	ret.beta = -alpha;
	return ret;
    }
    inline T value()
    { return best_value; }
};

template<typename T>
T try_cache_or_miss(const StandardGameState& state, vector<optional<T> >& cache, function<T(const StandardGameState& state)> miss) {
    Hash hash = state.get_hash();		
    int table_size = cache.size();
    auto idx = hash % table_size;
    auto maybe_element = cache[idx];    
    if (!!maybe_element) {
	return *maybe_element;
    } else {
	T move_value = miss(state);
	cache[idx] = move_value;
	return move_value;
    }
}

template<typename Value, typename SearchProcedure>
Value generalized_tree_fold(
    const StandardGameState& initial,
    int depth,
    vector<optional<Value> >& cache,
    SearchProcedure search
) {
    for (const Move& move : StandardRulesEngine::available_moves(initial)) {
	Value move_value;
	initial.peek_move(move, [&] (const StandardGameState& new_state) {
	    move_value =
		try_cache_or_miss<Value>(new_state, cache, [&] (const StandardGameState& state) {
		    if (depth == 0) {
		        return search.on_leaf_node(state);
		    } else {
			Value value = generalized_tree_fold<Value, SearchProcedure>(
			    state,
			    depth-1,
			    cache,
			    search.next()
			);
			return search.on_branch_node(state, value);
		    }
	        });
	});
	search.accumulate(move_value);
	if (search.should_cut())
	    break;
    }
    return search.value(); 
}

