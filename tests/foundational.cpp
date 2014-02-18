#undef __STRICT_ANSI__
#include "../utility.hpp"
#include "../direction.hpp"
#include "../parsing.hpp"
#include "../position.hpp"
#include "../test.hpp"
#include "../scoring.hpp"
#include "../minimax.hpp"
#include "../uint128_t.hpp"
#include "../rules-engines/bitboard.hpp"
#include <iostream>
#define BOOST_TEST_MAIN 1

// 'put_env' is not exported with strict ansi set
#include <cstdlib>

#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <iomanip>
using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE( position_internal_value ) {
    auto position = Position(4, 5); // Should be the center red soldier
    assert_eq(position.value, (uint8_t)31, "Position internal value is wrong");
}

BOOST_AUTO_TEST_CASE( operations_on_uint128_t ) {
    uint128_t derp = uint128_t(0x00F0FFFF0000FFFF, 0x0F00FFFF0000FFFF);
    assert_eq(lsb_first_set(derp), (uint8_t)0, "First one wrong 1");
    assert_eq(msb_first_set(derp), (uint8_t)119, "Second one wrong 1");
    assert_eq(num_set(derp),       (uint8_t)72, "Number of bits set is wrong 1");
    assert_eq(derp, derp, "Boards should equal themselves");
    assert_eq(derp & derp, derp, "& operation not idempotent");

    _assert(derp.get(15), "Bit not set 1");
    deny   (derp.get(16), "Bit set 1");
    _assert(derp.get(119), "Bit not set 2");
    deny   (derp.get(120), "Bit set 2");

    uint128_t herp = uint128_t(0x00000000FFF00000, 0x0000000000000000);
    assert_eq(lsb_first_set(herp), (uint8_t)83, "First one wrong 2");
    assert_eq(msb_first_set(herp), (uint8_t)95, "Second one wrong 2");
    assert_eq(num_set(herp),       (uint8_t)12, "Number of bits set is wrong 2");

    uint128_t zerp = uint128_t(0x0000000000000000, 0x00000000FFF00000);
    assert_eq(lsb_first_set(zerp), (uint8_t)20, "First one wrong 3");
    assert_eq(msb_first_set(zerp), (uint8_t)31, "Second one wrong 3");
    assert_eq(num_set(herp),       (uint8_t)12, "Number of bits set is wrong 3");

    uint128_t debugging_and = uint128_t(0, 1099511627776);
    bitboard  debug_board;
    debug_board.set(40);
    assert_eq(debug_board.msb, debugging_and.msb, "MSBs don't match");
    assert_eq(debug_board.lsb, debugging_and.lsb, "LSBs don't match");
    assert_eq(debugging_and & debug_board, debugging_and, "Boards don't match");

    _assert(!!debugging_and, "Board should be non-zero");
    _assert(!!(debugging_and & debug_board), "Board after & should be non-zero");

    uint128_t nerp = uint128_t(50, 70);
    uint128_t lerp;
    lerp = nerp;
    BOOST_REQUIRE_EQUAL(nerp, lerp);
}

BOOST_AUTO_TEST_CASE( bitboard_clear ) {
    bitboard x = uint128_t(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
    BOOST_CHECK_EQUAL((int)num_set(x), 128L);
    x.clear(50);
    BOOST_CHECK_EQUAL((int)num_set(x), 127L);
    x.clear(100);
    BOOST_CHECK_EQUAL((int)num_set(x), 126L);
}

BOOST_AUTO_TEST_CASE( parsing ) {
    auto c1 = parse_value<char>("103");
    _assert(c1, "103 doesn't parse");
    assert_eq((*c1).first, '1', "1 doesn't parse correctly");
    assert_eq((*c1).second, string("03"), "03 isn't the remaining text");

    auto i1 = parse_value<int>("0");
    assert_eq((*i1).first, 0, "0 doesn't parse correctly");

    auto i2 = parse_value<int>("103");
    _assert(i2, "103 doesn't parse");
    assert_eq((*i2).first, 103, "103 doesn't parse correctly");

    auto u1 = parse_until("1234", [] (char c) {
	    return c != '1' && c != '2';
	});
    _assert(u1, "1234 doesn't parse at all");
    assert_eq((*u1).first, string("12"), "12 not parsed");
    assert_eq((*u1).second, string("34"), "34 not parsed");

    auto u2 = parse_until("derp", [] (char) { return true; });
    deny(u2, "u2 parsed");

    auto r1 = parse_rank("5");
    _assert(r1, "5 doesn't parse as a rank");

    auto r2 = parse_rank("10");
    _assert(r2, "10 doesn't parse as a rank");

    auto r3 = parse_rank("0");
    deny(r3, "0 shouldn't parse as a rank");

    auto f1 = parse_file("A");
    _assert(f1, "A doesn't parse as a file");

    auto f2 = parse_file("J");
    deny(f2, "J shouldn't parse as a file");

    auto e8_position = Position(8, 5);

    auto p1 = parse_position("E8");
    _assert(p1, "E8 doesn't parse as a position");
    assert_eq((*p1).first, e8_position, "E8 parsed incorrectly");

    auto p2 = parse_position("derp");
    deny(p2, "'derp' parsed as a position");

    auto m1 = parse_move("e8E7");
    _assert(m1, "E8E7 doesn't parse as a move");
    assert_eq((*m1).first, Move(e8_position, Position(7, 5)), "E8E7 parsed incorrectly");

    auto m2 = parse_move("derp");
    deny(m2, "'derp' parsed as a move");

    auto tk1 = parse_token("Herp|Derp", '|');
    _assert(tk1, "Herp|Derp doesn't split correctly");
    assert_eq((*tk1).first, string("Herp"), "Herp not parsed correctly");
    assert_eq((*tk1).second, string("Derp"), "Incorrect remaining text");

    auto tk2 = parse_token("move E1E8", ' ');
    _assert(tk2, "move E1E8 doesn't split correctly");
    assert_eq((*tk2).first, string("move"), "move not parsed correctly");
    assert_eq((*tk2).second, string("E1E8"), "E1E8 not parsed correctly");
}
