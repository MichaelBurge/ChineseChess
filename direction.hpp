#pragma once

#include "position.hpp"
#include <functional>
using namespace std;

enum Direction { NORTH, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST };

Direction rotate_left(Direction direction);
Direction rotate_right(Direction direction);
Direction opposite_direction(Direction direction);
Position  move_direction(const Position& position, Direction direction);
Position  multi_move_direction(const Position&, Direction);
void with_90_degree_rotations(Direction direction, function<void(Direction)> action);
void shoot_ray_in_direction_until_should_stop(const Position& center, Direction direction, function<bool(Position)> should_stop);
