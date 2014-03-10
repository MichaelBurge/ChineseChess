#pragma once

#include "configuration.hpp"

const int TRANSPOSITION_TABLE_SIZE = 1000000;

extern void enumerate_tree(const StandardGameState&, int, function<void(const StandardGameState&)>);
extern int negamax(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);

template<typename T>
struct CachedValue {
    enum Flag {
	EXACT,
	LOWER_BOUND,
	UPPER_BOUND,
    };
    static const Hash Empty_Value = numeric_limits<Hash>::max();
    Hash hash;
    T value;
    Flag flag;
    int depth;

    CachedValue() : hash(Empty_Value) { }
    CachedValue(const Hash& hash, const T& value, Flag flag, int depth)
	: hash(hash),
	  value(value),
	  flag(flag),
	  depth(depth)
          {}

    inline bool is_cache_hit(Hash hash) const
    { return this->hash == hash; }
};

template<typename T>
struct MinimaxSearch {
    typedef function<T(const StandardGameState&)> Valuation;
    const StandardGameState& state;
    int depth;
    T best_value;
    T alpha_orig; // Lower bound at start of tree fold
    T alpha; // Updated lower bound as search progresses
    T beta; // Updated upper bound as search progresses
    Valuation valuation;
    bool _is_debug_output_enabled;

    MinimaxSearch(const StandardGameState& state, int depth, Valuation valuation, bool _is_debug_output_enabled = false) :
	state(state),
	depth(depth),
	best_value(numeric_limits<T>::min() + 1),
	alpha(numeric_limits<T>::min() + 1),
	beta(numeric_limits<T>::max()),
	valuation(valuation),
	_is_debug_output_enabled(_is_debug_output_enabled)
        {}

    inline void begin_depth()
    { alpha_orig = alpha; }

    inline void use_lower_bound(const T& value)
    { alpha = max(alpha, value); }

    inline void use_upper_bound(const T& value)
    { beta = min(beta, value); }
	    
    inline bool is_debug_output_enabled()
    { return _is_debug_output_enabled; }

    inline int identity()
    { return 0; }

    inline bool should_cut() {
	return alpha >= beta;
    }

    inline T on_leaf_node() {
	return valuation(state);
    }

    inline T on_branch_node(const StandardGameState&, const T& value) {
	T negamax_value = -value;
	alpha = max(alpha, negamax_value);
	return negamax_value;
    }

    inline void accumulate(const T& value) {
	if (value > best_value)
	    best_value = value;
    }

    inline MinimaxSearch<T> next(const StandardGameState& new_state) {
	// Debug output is only enabled for the outermost search parameter
	MinimaxSearch<T> ret(new_state, depth - 1, valuation, false);
	ret.alpha = -beta;
	ret.beta = -alpha;
	return ret;
    }
    inline T value()
    { return best_value; }


    inline CachedValue<T> get_cached_value() {
	typedef CachedValue<T> cv;

	typename cv::Flag flag;
	if (value() < alpha_orig)
	    flag = cv::UPPER_BOUND;
	else if (value() >= beta)
	    flag = cv::LOWER_BOUND;
	else
	    flag = cv::EXACT;

	return cv(state.get_hash(), value(), flag, depth);
    }
};

template<class T>
struct TranspositionTable {
    uint64_t num_cache_hits;
    uint64_t num_cache_hits_exact;
    uint64_t num_cache_hits_lower;
    uint64_t num_cache_hits_upper;
    uint64_t num_cache_misses;
    uint64_t num_cache_requests;
    vector<CachedValue<T> > cache;

    TranspositionTable(uint32_t table_size = TRANSPOSITION_TABLE_SIZE)
	: num_cache_hits(0),
	  num_cache_hits_exact(0),
	  num_cache_hits_lower(0),
	  num_cache_hits_upper(0),
	  num_cache_misses(0),
	  num_cache_requests(0),
	  cache(table_size)
	  { }

    inline void report_statistics() {
	cout << "Cache hits: "              << num_cache_hits << endl;
	cout << "Cache hits(exact): "       << num_cache_hits_exact << endl;
	cout << "Cache hits(lower bound): " << num_cache_hits_lower << endl;
	cout << "Cache hits(upper bound): " << num_cache_hits_upper << endl;
	cout << "Cache misses: "            << num_cache_misses << endl;
	cout << "Cache requests: "          << num_cache_requests << endl;
    }

    inline int index(Hash hash)
    { return hash % cache.size(); }

    template<class SearchProcedure>
    T try_cache_or_miss(const StandardGameState& state, SearchProcedure& search, function<T(const StandardGameState& state)> miss) {
	typedef CachedValue<T> cv;
	num_cache_requests++;

	Hash hash = state.get_hash();		
	auto idx = index(hash);
	auto cached_element = cache[idx];
	bool is_cache_hit = cached_element.is_cache_hit(hash);

        if (is_cache_hit) {
	    num_cache_hits++;
	    switch (cached_element.flag) {
	    case cv::EXACT:
		num_cache_hits_exact++;
		return cached_element.value;
	    case cv::LOWER_BOUND:
		num_cache_hits_lower++;
		search.use_lower_bound(cached_element.value);
		break;
	    case cv::UPPER_BOUND:
		num_cache_hits_upper++;
		search.use_upper_bound(cached_element.value);
		break;
	    }
	    if (search.should_cut())
		return cached_element.value;
	} else {
	    num_cache_misses++;
	}

	T move_value = miss(state);
	cache[idx] = search.get_cached_value();
	return move_value;
    }

    void store_value(const CachedValue<T>& value) {
	auto idx = index(value.hash);
	auto cached_element = cache[idx];
	bool is_cache_hit = value.is_cache_hit(value.hash);
	if (!is_cache_hit || (cached_element.depth <= value.depth)) {
	    cache[idx] = value;
	}
    }
};

template<typename Value, typename SearchProcedure>
Value generalized_tree_fold(
    TranspositionTable<Value>& cache,
    SearchProcedure search
) {

    auto process_branch_node = [&] (const StandardGameState& state) {
	Value value = generalized_tree_fold<Value, SearchProcedure>(
	    cache,
	    search.next(state)
	);
	return search.on_branch_node(state, value);	
    };

    if (search.depth == 0) {
	return search.on_leaf_node();
    }

    for (const Move& move : StandardRulesEngine::available_moves(search.state)) {
	Value move_value;
	search.state.peek_move(move, [&] (const StandardGameState& new_state) {
            move_value = cache.try_cache_or_miss(
		new_state,
		search,
		process_branch_node
	    );
	});
	search.accumulate(move_value);
	if (search.should_cut())
	    break;
    }
    cache.store_value(search.get_cached_value());

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
extern void print_analysis(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);
