#include "gamestate.hpp"

ostream& operator<<(ostream& os, const ReferenceGameStateArrayStorage& board) {
    auto draw_river = [] () {
	for (int i = 1; i != 9; i++)
	    cout << "~~";
        cout << "~" << endl;
    };

    auto draw_rank = [&] (int rank) {
	for (int i = 1; i != 10; i++)
            cout << character_for_piece(board.get_piece(Position(rank, i))) << ' ';
        cout << endl;
    };

    for (int i = 10; i != 0; i--) {
        if (i == 5)
            draw_river();
        draw_rank(i);
    };
    return os;
}
