#include "rules.hpp"
#include <cstdint>
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

template<typename T, typename U> void assert_eq(const T& actual, const U& expected, string message) {
  assert(
      actual == static_cast<T>(expected),
      message + "- values: (Actual: " + to_string(actual) + "; Expected: " + to_string(expected) + ")"
  );
}

template void assert_eq(const uint32_t&, const uint32_t&, string);
template void assert_eq(const uint32_t&, const int32_t&, string);
template void assert_eq(const uint64_t&, const int32_t&, string);
