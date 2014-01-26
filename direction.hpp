#pragma once

enum Direction { NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST };

Direction rotate_left(Direction direction);
Direction rotate_right(Direction direction);
Direction opposite_direction(Direction direction);
Position  move_direction(const Position& position, Direction direction);
