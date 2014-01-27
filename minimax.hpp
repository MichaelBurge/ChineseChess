#pragma once

#include "rules.hpp"

Move best_move(const GameState& state, int depth, function<int(const GameState&)> value);
vector<pair<Move, int> > move_scores(const GameState& state, int depth, function<int(const GameState&)> value);
void print_move_scores(const vector<pair<Move, int> >& scores);
