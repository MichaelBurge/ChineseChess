#include <stdexcept>
using namespace std;

class unimplemented : public logic_error {
public:
  unimplemented(const string & message) throw() : logic_error(message) {}
};
