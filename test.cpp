#include "rules.hpp"
#include <iostream>
#include <map>
#include <stdexcept>
using namespace std;

void assert(bool condition, string message) {
  if (condition)
    return;
  cerr << "Failing test! Message: " << message << endl;
  throw logic_error("Assertion failure");
}

template<typename T, typename U> void assert_eq(const T& a, const U& b, string message) {
  assert(a == b, message + "- values: (" + to_string(a) + "," + to_string(b) + ")");
}

template void assert_eq(const uint32_t&, const uint32_t&, string);
template void assert_eq(const uint32_t&, const int32_t&, string);
template void assert_eq(const uint64_t&, const int32_t&, string);
