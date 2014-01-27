#pragma once

#include <ostream>
using namespace std;

struct Position {
  int rank;
  int file;
  bool operator<(const Position& a) const;
  bool operator==(const Position& a) const;
};
ostream& operator<<(ostream& os, const Position&);

extern Position mkPosition(int rank, int file);
