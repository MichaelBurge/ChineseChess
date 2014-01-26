#pragma once

#include <map>
#include <vector>
#include "position.hpp"
using namespace std;

enum PieceType { EMPTY, GENERAL, ADVISOR, ELEPHANT, HORSE, CHARIOT, CANNON, SOLDIER };
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

extern GameState    new_game();
extern vector<Move> available_moves(const GameState &);
extern void         apply_move(GameState &, const Move&);
extern void         state_to_board(GameState &, Piece*& board_ref);
extern GameState    mkState(Player current_turn);
extern Move         mkMove(const Position& from, const Position& to);
extern Piece        mkPiece(PieceType type, Player owner);
extern void         insert_piece(GameState &, const Position&, const Piece&);
extern bool         is_capture(const GameState&, const Move&);

extern int          num_available_moves(const GameState&);
extern int          num_available_captures(const GameState&);
