#pragma once

#include "move.hpp"
#include "player.hpp"
#include "piece.hpp"

template<class Implementation>
struct GameState
{
    GameState(Player _current_turn) : implementation(Implementation(_current_turn)) {}

    Piece get_piece(const Position& position) const
    { return implementation.get_piece(position); }

    void insert_piece(const Position& position, const Piece& piece)
    { implementation.insert_piece(position, piece); }

    void remove_piece(const Position& position)
    { implementation.remove_piece(position); }

    void for_each_piece(function<void(Position, Piece)> action) const
    { implementation.for_each_piece(action); }

    void apply_move(const Move& move)
    { implementation.apply_move(move); }

    void peek_move(const Move& move, const function<void(const GameState&)> action) const
    {
	implementation.peek_move(move, [&] (const Implementation& impl) {
	    action(const_cast<GameState&>(*this));
	});
    }	

    void current_turn(Player player)
    { implementation.current_turn(player); }

    Player current_turn() const
    { return implementation.current_turn(); }

    void switch_turn()
    { implementation.switch_turn(); }

    void print_debug_board()
    { implementation.print_debug_board(); }

    template<class T>
    friend ostream& operator<<(ostream&, const GameState<T>&);

    static GameState new_game()
    {
	auto state = GameState(RED);

	auto rank = 1;
	state.insert_piece(Position(rank, 1), RED_CHARIOT);
	state.insert_piece(Position(rank, 2), RED_HORSE);
	state.insert_piece(Position(rank, 3), RED_ELEPHANT);
	state.insert_piece(Position(rank, 4), RED_ADVISOR);
	state.insert_piece(Position(rank, 5), RED_GENERAL);
	state.insert_piece(Position(rank, 6), RED_ADVISOR);
	state.insert_piece(Position(rank, 7), RED_ELEPHANT);
	state.insert_piece(Position(rank, 8), RED_HORSE);
	state.insert_piece(Position(rank, 9), RED_CHARIOT);

	rank = 3;
	state.insert_piece(Position(rank, 2), RED_CANNON);
	state.insert_piece(Position(rank, 8), RED_CANNON);

	rank = 4;
	state.insert_piece(Position(rank, 1), RED_SOLDIER);
	state.insert_piece(Position(rank, 3), RED_SOLDIER);
	state.insert_piece(Position(rank, 5), RED_SOLDIER);
	state.insert_piece(Position(rank, 7), RED_SOLDIER);
	state.insert_piece(Position(rank, 9), RED_SOLDIER);

	rank = 10;
	state.insert_piece(Position(rank, 1), BLACK_CHARIOT);
	state.insert_piece(Position(rank, 2), BLACK_HORSE);
	state.insert_piece(Position(rank, 3), BLACK_ELEPHANT);
	state.insert_piece(Position(rank, 4), BLACK_ADVISOR);
	state.insert_piece(Position(rank, 5), BLACK_GENERAL);
	state.insert_piece(Position(rank, 6), BLACK_ADVISOR);
	state.insert_piece(Position(rank, 7), BLACK_ELEPHANT);
	state.insert_piece(Position(rank, 8), BLACK_HORSE);
	state.insert_piece(Position(rank, 9), BLACK_CHARIOT);
    
	rank = 8;
	state.insert_piece(Position(rank, 2), BLACK_CANNON);
	state.insert_piece(Position(rank, 8), BLACK_CANNON);

	rank = 7;
	state.insert_piece(Position(rank, 1), BLACK_SOLDIER);
	state.insert_piece(Position(rank, 3), BLACK_SOLDIER);
	state.insert_piece(Position(rank, 5), BLACK_SOLDIER);
	state.insert_piece(Position(rank, 7), BLACK_SOLDIER);
	state.insert_piece(Position(rank, 9), BLACK_SOLDIER);

	return state;
    }
    Implementation implementation;
};

template<class T>
ostream& operator<<(ostream& os, const GameState<T>& state)
{ return os << state.implementation; }


