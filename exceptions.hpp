#include <stdexcept>
#include "move.hpp"
using namespace std;

class unimplemented : public logic_error {
public:
    unimplemented(const string & message) throw() : logic_error(message) {}
};

class illegal_move : public logic_error {
public:
    illegal_move(const Move& move) throw() : logic_error("Illegal move: " + move_repr(move)) {}
};
