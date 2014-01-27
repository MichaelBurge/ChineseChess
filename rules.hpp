#pragma once

#include <boost/multi_array.hpp>
#include <boost/optional/optional.hpp>
#include <map>
#include <vector>
#include "position.hpp"
#include "direction.hpp"
#include "move.hpp"

using namespace std;
using namespace boost;

enum PieceType { EMPTY, GENERAL, ADVISOR, ELEPHANT, HORSE, CHARIOT, CANNON, SOLDIER };
enum Player { RED, BLACK };

struct Piece {
  PieceType piece_type;
  Player owner;
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
extern void         apply_move(GameState &, const Move&, bool check_legality = true);
extern void         insert_piece(GameState &, const Position&, const Piece&);
extern bool         is_capture(const GameState&, const Move&);
extern bool         is_legal_move(const GameState &, const Move&);
extern optional<Player> winner(const GameState&);
extern bool         is_king_in_check(const GameState& state, Player);
extern Player       next_player(Player);

// Debugging aids
extern bool violates_flying_kings_rule(const GameState&);
extern bool violates_kings_in_castle_rule(const GameState& state);
extern bool violates_can_only_capture_enemy_pieces_rule(const GameState& state, const Move& move);
extern bool is_invalid_state(const GameState &);
extern bool is_position_valid(const Position &);

// Querying state for useful information
template<typename T> T peek_move(const GameState& state, Move move, bool check_legality, const function<T(const GameState &)>& action);
extern vector<Position> filter_pieces(const GameState& state, function<bool(Position, Piece)> pred);
extern vector<Position> filter_pieces_by_type(const GameState& state, PieceType type);
extern vector<Move> filter_available_moves(const GameState& state, function<bool(const Move&)> pred);
extern vector<Move> filter_available_captures(const GameState& state, function<bool(const Move&)> pred);
extern vector<Move> captures_for_position(const GameState& state, const Position& position);
extern vector<Move> available_moves_from(const GameState& state, const Position& position);
extern void for_each_piece(const GameState& state, function<void(Position, Piece)> action);
extern int          num_available_moves(const GameState&);
extern int          num_available_captures(const GameState&);

// Output
extern multi_array<Piece, 2> state_to_board(const GameState &);
extern void print_board(const GameState &);
extern void print_available_moves(const GameState &);
