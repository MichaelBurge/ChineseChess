#include "minimax.hpp"

const int lowest = numeric_limits<int>::min();
const int highest = numeric_limits<int>::max();

 // http://en.wikipedia.org/wiki/Negamax#NegaMax_with_Alpha_Beta_Pruning
int negamax(const GameState& state, int depth, int& node_count, int alpha, int beta, int color, function<int(const GameState&)> valuation) {
    auto node_value = color * valuation(state);
    node_count--;
    auto hit_maximum_nodes = [&] () { return node_count <= 0; };
    if (hit_maximum_nodes())
        return node_value;
    if (depth == 0)
        return node_value;
    auto moves = available_moves(state);
    if (moves.empty())
        return node_value;
    int best_value = lowest;
    for (auto i = moves.begin(); i != moves.end(); i++) {
        if (hit_maximum_nodes())
            return best_value;
        auto move = *i;
        peek_move<void>(state, move, false, [&] (const GameState& newState) -> void {
            auto val = -negamax(newState, depth - 1, node_count, -beta, -alpha, -color, valuation);
            best_value = max(best_value, val);
            alpha = max(alpha, val);
            if (alpha > beta)
                i = moves.end(); // break
            return;
        });
    }
    return best_value;
}

void map_negamax(const GameState& state, int depth, int node_count, function<int(const GameState&)> valuation, function<void(const Move&, int value)> action) {
    auto moves = available_moves(state);
    if (moves.empty())
        throw logic_error("No move exists");
    int color =
        state.current_turn == RED
          ?  1
          : -1;
    for_each(moves.begin(), moves.end(), [&] (const Move& move) {
        peek_move<void>(state, move, false, [&] (const GameState& newState) {
            auto value = color * negamax(newState, depth, node_count, lowest, highest, color, valuation);
            action(move, value);
        });
    });
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

vector<pair<Move, int> > move_scores(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> valuation) {
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
