#pragma once

#include "position.hpp"
#include <functional>
using namespace std;

enum Direction { NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST };

Direction rotate_left(Direction direction);
Direction rotate_right(Direction direction);
Direction opposite_direction(Direction direction);
Position  multi_move_direction(const Position&, Direction);
void with_90_degree_rotations(Direction direction, function<void(Direction)> action);
void shoot_ray_in_direction_until_should_stop(const Position& center, Direction direction, function<bool(Position)> should_stop);

inline Position move_direction(const Position& position, Direction direction) {
    switch (direction) {
    case NORTH:
	return Position(position.rank() + 1, position.file());
    case SOUTH:
	return Position(position.rank() - 1, position.file());
    case WEST:
	return Position(position.rank(), position.file() + 1);
    case EAST:
	return Position(position.rank(), position.file() - 1);
    case SOUTHEAST:
	return Position(position.rank() - 1, position.file() - 1);
    case SOUTHWEST:
	return Position(position.rank() - 1, position.file() + 1);
    case NORTHEAST:
	return Position(position.rank() + 1, position.file() - 1);
    case NORTHWEST:
	return Position(position.rank() + 1, position.file() + 1);
    }
    __builtin_unreachable();
}
