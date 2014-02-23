#include "position.hpp"
#include "direction.hpp"
#include <stdexcept>
using namespace std;

Direction rotate_left(Direction direction) {
  switch (direction) {
  case NORTH:
    return NORTHWEST;
  case NORTHWEST:
    return WEST;
  case WEST:
    return SOUTHWEST;
  case SOUTHWEST:
    return SOUTH;
  case SOUTH:
    return SOUTHEAST;
  case SOUTHEAST:
    return EAST;
  case EAST:
    return NORTHEAST;
  case NORTHEAST:
    return NORTH;
  default:
    throw logic_error("Unknown direction");
  }
}

Direction rotate_right(Direction direction) {
  switch (direction) {
  case NORTH:
    return NORTHEAST;
  case NORTHEAST:
    return EAST;
  case EAST:
    return SOUTHEAST;
  case SOUTHEAST:
    return SOUTH;
  case SOUTH:
    return SOUTHWEST;
  case SOUTHWEST:
    return WEST;
  case WEST:
    return NORTHWEST;
  case NORTHWEST:
    return NORTH;
  default:
    throw logic_error("Unknown direction");
  }
}

Direction rotate_90_left(Direction direction) {
  return rotate_left(rotate_left(direction));
}

Direction opposite_direction(Direction direction) {
  return rotate_90_left(rotate_90_left(direction));
}

void with_90_degree_rotations(Direction direction, function<void(Direction)> action) {
  action(direction);
  action(direction = rotate_90_left(direction));
  action(direction = rotate_90_left(direction));
  action(direction = rotate_90_left(direction));
}

void shoot_ray_in_direction_until_should_stop(const Position& center, Direction direction, function<bool(Position)> should_stop) {
  Position candidate = center;
  do {
    candidate = move_direction(candidate, direction);
  } while (!should_stop(candidate));
}

Position  multi_move_direction(const Position& position, Direction direction) {
    return move_direction(move_direction(position, direction), direction);
}
