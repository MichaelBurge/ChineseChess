#pragma once

struct Position {
  int rank;
  int file;
  bool operator<(const Position& a) const;
  bool operator==(const Position& a) const;
};

extern Position mkPosition(int rank, int file);
