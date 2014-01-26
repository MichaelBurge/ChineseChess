#pragma once

#include <string>
#include <stdint.h>
using namespace std;

extern void _assert(bool condition, string message);
template<typename T, typename U> extern void assert_eq(const T& a, const U& b, string message);

