#pragma once

#include "gamestate.hpp"
#include "move.hpp"
#include "rules-engine.hpp"
#include "configuration.hpp"

extern Move best_move(const StandardGameState& state, int depth, int max_nodes, function<int(const StandardGameState&)> valuation);
extern int negamax(const StandardGameState& state, int depth, int& node_count, function<int(const StandardGameState&)> valuation);
extern vector<pair<Move, int> > move_scores(const StandardGameState& state, function<int(const StandardGameState&)> valuation);
extern vector<pair<Move, int> > move_scores_minimax(const StandardGameState& state, int depth, int max_nodes, function<int(const StandardGameState&)> valuation);
extern void print_move_scores(const vector<pair<Move, int> >& scores);
extern vector<Move> best_move_sequence(const StandardGameState& state, int depth, function<int(const StandardGameState&)> valuation);
