#pragma once

#include <map>
#include <set>
#include "position.hpp"
using namespace std;

enum PieceType { EMPTY, GENERAL, ADVISOR, ELEPHANT, HORSE, CHARIOT, CANON, SOLDIER };
enum Player { RED, BLACK };

struct Piece {
  PieceType pieceType;
  Player owner;
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
extern set<Move> available_moves_for_piece(GameState &, Position, Piece);
extern set<Move> available_moves_for_general(GameState &, Position, Player);
extern void      apply_move(GameState &, Move);
extern void      state_to_board(GameState &, Piece*& board_ref);
extern Move      mkMove(const Position& from, const Position& to);
extern Piece     mkPiece(PieceType type, Player owner);
