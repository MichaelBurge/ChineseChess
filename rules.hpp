#pragma once

#include <boost/multi_array.hpp>
#include <map>
#include <vector>
#include "position.hpp"
#include "direction.hpp"

using namespace std;
using namespace boost;

enum PieceType { EMPTY, GENERAL, ADVISOR, ELEPHANT, HORSE, CHARIOT, CANNON, SOLDIER };
enum Player { RED, BLACK };

struct Piece {
  PieceType piece_type;
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

extern GameState    mkState(Player current_turn);
extern Move         mkMove(const Position& from, const Position& to);
extern Move         mkMove(const Position& from, Direction direction);
extern Piece        mkPiece(PieceType type, Player owner);

// Basic game rules
extern GameState    new_game();
extern vector<Move> available_moves(const GameState &);
extern void         apply_move(GameState &, const Move&);
extern void         insert_piece(GameState &, const Position&, const Piece&);
extern bool         is_capture(const GameState&, const Move&);

// Querying state for useful information
extern multi_array<Piece, 2> state_to_board(GameState &, Piece*& board_ref);
void print_board(const GameState &);
template<typename T> T peek_move(const GameState& state, Move move, const function<T(const GameState &)>& action);
extern vector<Position> filter_pieces(const GameState& state, function<bool(Position, Piece)> pred);
extern vector<Position> filter_pieces_by_type(const GameState& state, PieceType type);
extern int          num_available_moves(const GameState&);
extern int          num_available_captures(const GameState&);
