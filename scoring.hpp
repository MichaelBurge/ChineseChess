#pragma once

#include "rules.hpp"

extern int standard_score_function(const GameState& state);
extern int piece_value(PieceType piece_type);
extern int piece_score(const GameState& state);
