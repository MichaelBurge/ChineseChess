#pragma once

#include <map>
#include <set>

using namespace std;

enum PieceType { EMPTY, GENERAL, ADVISOR, ELEPHANT, HORSE, CHARIOT, CANON, SOLDIER };

typedef int Player;

struct Piece {
  PieceType type;
  Player owner;
};

struct Position {
  int rank;
  int file;
};

struct Move {
  Position from;
  Position to;
};

struct GameState {
  map<Position, Piece> pieces;
  Player current_turn;
};

extern GameState new_game();
extern set<Move> available_moves(GameState &, Player);
extern void      state_to_board(GameState &, Piece*& board_ref);
