#pragma once

#include "rules.hpp"

extern Move best_move(const GameState& state, int depth, int max_nodes, function<int(const GameState&)> value);
extern vector<pair<Move, int> > move_scores(const GameState& state, function<int(const GameState&)> value);
extern void print_move_scores(const vector<pair<Move, int> >& scores);
