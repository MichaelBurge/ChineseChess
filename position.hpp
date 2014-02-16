#pragma once

#include <ostream>
#include <stdint.h>
using namespace std;

struct Position {
    Position(uint8_t rank, uint8_t file);
    uint8_t rank() const;
    uint8_t file() const;
    bool operator<(const Position& a) const;
    bool operator==(const Position& a) const;
    bool is_valid() const;
private:
    uint8_t value;
};

ostream& operator<<(ostream& os, const Position&);
string position_repr(const Position&);
