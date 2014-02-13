#pragma once

#include "rules.hpp"

extern Move best_move(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> valuation);
extern int negamax(const GameState& state, int depth, int& node_count, function<int(const GameState&)> valuation);
extern vector<pair<Move, int> > move_scores(const GameState& state, function<int(const GameState&)> valuation);
extern vector<pair<Move, int> > move_scores_minimax(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> valuation);
extern void print_move_scores(const vector<pair<Move, int> >& scores);
extern vector<Move> best_move_sequence(const GameState& state, int depth, function<int(const GameState&)> valuation);
