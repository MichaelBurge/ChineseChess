#include "../rules-engine.hpp"
#include "../minimax.hpp"
#include "../scoring.hpp"
#include "../configuration.hpp"
using namespace std;

int main() {
    auto state = StandardGameState::new_game();
    best_move(state, 2, 100000, piece_score);
    return 0;
}
