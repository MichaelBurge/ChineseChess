#include "rules.hpp"
#include <cstdint>
#include <iostream>
#include <map>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
using namespace std;

void _assert(bool condition, string message) {
  if (condition)
    return;
  cerr << "Failing test! Message: " << message << endl;
  throw logic_error("Assertion failure");
}

void deny(bool condition, string message) {
    _assert(!condition, message);
}

template<typename T, typename U> void assert_eq(const T& actual, const U& expected, string message) {
  _assert(
      actual == static_cast<T>(expected),
      message + "- values: (Actual: " + lexical_cast<string>(actual) + "; Expected: " + lexical_cast<string>(expected) + ")"
  );
}

template void assert_eq(const char&, const char&, string);
template void assert_eq(const uint32_t&, const uint32_t&, string);
template void assert_eq(const uint32_t&, const int32_t&, string);
template void assert_eq(const uint64_t&, const int32_t&, string);
template void assert_eq(const uint64_t&, const uint64_t&, string);
template void assert_eq(const int32_t&,  const int32_t&, string);
template<> void assert_eq(const Position& actual, const Position& expected, string message) {
    assert_eq(actual.rank, expected.rank, message + " - Rank differs");
    assert_eq(actual.file, expected.file, message + " - File differs");
}

template<> void assert_eq(const Move& actual, const Move& expected, string message) {
    assert_eq(actual.from, expected.from, message + " - From differs");
    assert_eq(actual.to, expected.to, message + " - To differs");
}

template<> void assert_eq(const string& actual, const string& expected, string message) {
    _assert(
        actual == expected,
        message + " - values: (Actual: " + actual + "; Expected: " + expected + ")");
}
