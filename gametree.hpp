#pragma once

#include "configuration.hpp"

const int TRANSPOSITION_TABLE_SIZE = 1000000;

extern void enumerate_tree(const StandardGameState&, int, function<void(const StandardGameState&)>);
extern int negamax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);

template<typename T>
struct MinimaxSearch {
    typedef function<T(const StandardGameState&)> Valuation;
    T best_value;
    T alpha;
    T beta;
    Valuation valuation;
    bool _is_debug_output_enabled;

    MinimaxSearch(Valuation valuation, bool _is_debug_output_enabled = false) :
	best_value(numeric_limits<T>::min() + 1),
	alpha(numeric_limits<T>::min() + 1),
	beta(numeric_limits<T>::max()),
	valuation(valuation),
	_is_debug_output_enabled(_is_debug_output_enabled)
        {}

    inline bool is_debug_output_enabled()
    { return _is_debug_output_enabled; }

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
struct CachedValue {
    static const Hash Empty_Value = numeric_limits<Hash>::max();
    Hash hash;
    T value;
    CachedValue() : hash(Empty_Value) { }
    CachedValue(const Hash& hash, const T& value)
	: hash(hash),
	  value(value)
          {}

    inline bool is_empty()
    { return value == Empty_Value; }
};

template<class T>
struct TranspositionTable {
    uint64_t num_cache_hits;
    uint64_t num_cache_misses;
    uint64_t num_cache_requests;
    vector<CachedValue<T> > cache;

    TranspositionTable(uint32_t table_size = TRANSPOSITION_TABLE_SIZE)
	: num_cache_hits(0),
	  num_cache_misses(0),
	  num_cache_requests(0),
	  cache(table_size)
	  { }

    inline void report_statistics() {
	cout << "Cache hits: " << num_cache_hits << endl;
	cout << "Cache misses: " << num_cache_misses << endl;
	cout << "Cache requests: " << num_cache_requests << endl;
    }

    T try_cache_or_miss(const StandardGameState& state, function<T(const StandardGameState& state)> miss) {
	num_cache_requests++;

	Hash hash = state.get_hash();		
	int table_size = cache.size();
	auto idx = hash % table_size;
	auto cached_element = cache[idx];
	if (cached_element.is_empty() || cached_element.hash != hash) {
	    num_cache_misses++;
	    T move_value = miss(state);
	    cache[idx] = CachedValue<T>(hash, move_value);
	    return move_value;
	} else {
	    num_cache_hits++;
	    return cached_element.value;
	}
    }
};

template<typename Value, typename SearchProcedure>
Value generalized_tree_fold(
    const StandardGameState& state,
    int depth,
    TranspositionTable<Value>& cache,
    SearchProcedure search
) {
    if (depth == 0) {
	return search.on_leaf_node(state);
    }
    auto process_branch_node = [&] (const StandardGameState& state) {
	Value value = generalized_tree_fold<Value, SearchProcedure>(
	    state,
	    depth-1,
	    cache,
	    search.next()
	    );
	return search.on_branch_node(state, value);	
    };

    for (const Move& move : StandardRulesEngine::available_moves(state)) {
	Value move_value;
	state.peek_move(move, [&] (const StandardGameState& new_state) {
            move_value = cache.try_cache_or_miss(new_state, process_branch_node);
	});
	search.accumulate(move_value);
	if (search.should_cut())
	    break;
    }
    if (search.is_debug_output_enabled()) {
	cache.report_statistics();
    }
    return search.value(); 
}

extern void map_negamax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation, function<void(const Move&, int value)> action);
extern Move best_move(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);
extern vector<Move> best_move_sequence(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);
extern void print_move_sequence(const vector<Move>& moves);
extern void print_move_scores(const vector<pair<Move, int> >& scores);
extern vector<pair<Move, int> > move_scores(const StandardGameState& state, function<int(const StandardGameState&)> valuation);
extern vector<pair<Move, int> > move_scores_minimax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);
