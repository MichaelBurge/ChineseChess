#include "../rules-engine.hpp"
#include "../gametree.hpp"
#include "../scoring.hpp"
#include "../configuration.hpp"
using namespace std;

int main() {
    auto state = StandardGameState::new_game();
    TranspositionTable<int> transposition_table(10000000);
    typedef MinimaxSearch<int> SearchProcedure;
    SearchProcedure search(piece_score, true);
    int score = generalized_tree_fold<int, SearchProcedure>(
	state,
	10,
	transposition_table,
	search
    );
    cout << "Score: " << score << endl;
    return 0;
}
