#include "../rules-engine.hpp"
#include "../minimax.hpp"
#include "../scoring.hpp"
using namespace std;

extern RulesEngine *rules;

int main() {
    auto state = rules->new_game();
    best_move(state, 2, 100000, piece_score);
    return 0;
}
