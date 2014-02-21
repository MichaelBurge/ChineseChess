#pragma once

#include "rules-engine.hpp"
#include "rules-engines/reference.hpp"
#include "rules-engines/reference-gamestate.hpp"
#include "rules-engines/bitboard-gamestate.hpp"
#include "rules-engines/bitboard-rules.hpp"
#include "rules-engines/bitboard.hpp"

typedef GameState<BitboardGameState> StandardBitboardGameState;
typedef GameState<ReferenceGameState> StandardReferenceGameState;

//typedef StandardReferenceGameState StandardGameState;
typedef StandardBitboardGameState StandardGameState;

typedef RulesEngine<StandardGameState, BitboardRules> StandardBitboardRulesEngine;
typedef RulesEngine<StandardGameState, ReferenceRules> StandardReferenceRulesEngine;


//typedef StandardReferenceRulesEngine StandardRulesEngine;
typedef StandardBitboardRulesEngine StandardRulesEngine;

#define ENABLE_DEBUG_CONSISTENCY_CHECKS 1
