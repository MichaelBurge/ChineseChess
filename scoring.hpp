#pragma once

#include "gamestate.hpp"
#include "configuration.hpp"

extern int standard_score_function(const StandardGameState& state);
extern int piece_score(const StandardGameState& state);
extern int piece_value(Piece piece_type);
