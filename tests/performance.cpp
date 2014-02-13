#include "../rules.hpp"
#include "../minimax.hpp"
#include "../scoring.hpp"
using namespace std;

int main() {
    auto state = new_game();
    best_move(state, 2, 100000, piece_score);
    return 0;
}
