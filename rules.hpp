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
  bool operator<(const Position& a) const;
  bool operator==(const Position& a) const;
};

struct Move {
  Position from;
  Position to;
  bool operator<(const Move& a) const;
};

struct GameState {
  map<Position, Piece> pieces;
  Player current_turn;
};

extern GameState new_game();
extern set<Move> available_moves(GameState &, Player);
extern void      apply_move(GameState &, Move);
extern void      state_to_board(GameState &, Piece*& board_ref);
