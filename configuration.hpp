#pragma once

#include "rules-engine.hpp"
#include "rules-engines/reference.hpp"
#include "rules-engines/reference-gamestate.hpp"

typedef GameState<ReferenceGameState> StandardGameState;
typedef RulesEngine<StandardGameState, ReferenceRules> StandardRulesEngine;
