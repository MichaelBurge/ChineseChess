#undef __STRICT_ANSI__
#include "../direction.hpp"
#include "../parsing.hpp"
#include "../position.hpp"
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
    BOOST_REQUIRE_EQUAL(position.value, (uint8_t)31);
}

BOOST_AUTO_TEST_CASE( operations_on_uint128_t ) {
    uint128_t derp = uint128_t(0x00F0FFFF0000FFFF, 0x0F00FFFF0000FFFF);
    BOOST_REQUIRE_EQUAL(lsb_first_set(derp), (uint8_t)0);
    BOOST_REQUIRE_EQUAL(msb_first_set(derp), (uint8_t)119);
    BOOST_REQUIRE_EQUAL(num_set(derp),       (uint8_t)72);
    BOOST_REQUIRE_EQUAL(derp, derp);
    BOOST_REQUIRE_EQUAL(derp & derp, derp);

    BOOST_REQUIRE( derp.get(15));
    BOOST_REQUIRE(!derp.get(16));
    BOOST_REQUIRE( derp.get(119));
    BOOST_REQUIRE(!derp.get(120));

    uint128_t herp = uint128_t(0x00000000FFF00000, 0x0000000000000000);
    BOOST_REQUIRE_EQUAL(lsb_first_set(herp), (uint8_t)84);
    BOOST_REQUIRE_EQUAL(msb_first_set(herp), (uint8_t)95);
    BOOST_REQUIRE_EQUAL(num_set(herp),       (uint8_t)12);

    uint128_t zerp = uint128_t(0x0000000000000000, 0x00000000FFF00000);
    BOOST_REQUIRE_EQUAL(lsb_first_set(zerp), (uint8_t)20);
    BOOST_REQUIRE_EQUAL(msb_first_set(zerp), (uint8_t)31);
    BOOST_REQUIRE_EQUAL(num_set(herp),       (uint8_t)12);

    uint128_t debugging_and = uint128_t(0, 1099511627776);
    bitboard  debug_board;
    debug_board.set(40);
    BOOST_REQUIRE_EQUAL(debug_board.msb, debugging_and.msb);
    BOOST_REQUIRE_EQUAL(debug_board.lsb, debugging_and.lsb);
    BOOST_REQUIRE_EQUAL(debugging_and & debug_board, debugging_and);

    BOOST_REQUIRE(!!debugging_and);
    BOOST_REQUIRE(!!(debugging_and & debug_board));

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

BOOST_AUTO_TEST_CASE( parsing_chars) {
    auto c1 = parse_value<char>("103");
    BOOST_REQUIRE(c1);
    BOOST_CHECK_EQUAL((*c1).first, '1');
    BOOST_CHECK_EQUAL((*c1).second, string("03"));
}

BOOST_AUTO_TEST_CASE( parsing_ints) {
    auto i1 = parse_value<int>("0");
    BOOST_REQUIRE_EQUAL((*i1).first, 0);

    auto i2 = parse_value<int>("103");
    BOOST_REQUIRE(i2);
    BOOST_REQUIRE_EQUAL((*i2).first, 103);
}

BOOST_AUTO_TEST_CASE( parsing_until ) {
    auto u1 = parse_until("1234", [] (char c) {
	return c != '1' && c != '2';
    });
    BOOST_REQUIRE(u1);
    BOOST_CHECK_EQUAL((*u1).first, string("12"));
    BOOST_CHECK_EQUAL((*u1).second, string("34"));

    auto u2 = parse_until("derp", [] (char) { return true; });
    BOOST_REQUIRE(!u2);
}

BOOST_AUTO_TEST_CASE( parsing_ranks ) {
    auto r1 = parse_rank("5");
    BOOST_REQUIRE(r1);

    auto r2 = parse_rank("10");
    BOOST_REQUIRE(r2);

    auto r3 = parse_rank("0");
    BOOST_REQUIRE(!r3);
}

BOOST_AUTO_TEST_CASE( parsing_files ) {
    auto f1 = parse_file("A");
    BOOST_REQUIRE(f1);

    auto f2 = parse_file("J");
    BOOST_REQUIRE(!f2);
}

BOOST_AUTO_TEST_CASE( parsing_positions) {
    auto e8_position = Position(8, 5);

    auto p1 = parse_position("E8");
    BOOST_REQUIRE(p1);
    BOOST_REQUIRE_EQUAL((*p1).first, e8_position);

    auto p2 = parse_position("derp");
    BOOST_REQUIRE(!p2);
}

BOOST_AUTO_TEST_CASE( parsing_moves ) {
    auto e8_position = Position(8, 5);
    auto m1 = parse_move("e8E7");
    BOOST_REQUIRE(m1);
    BOOST_REQUIRE_EQUAL((*m1).first, Move(e8_position, Position(7, 5)));

    auto m2 = parse_move("derp");
    BOOST_REQUIRE(!m2);

    auto tk1 = parse_token("Herp|Derp", '|');
    BOOST_REQUIRE(tk1);
    BOOST_REQUIRE_EQUAL((*tk1).first, string("Herp"));
    BOOST_REQUIRE_EQUAL((*tk1).second, string("Derp"));

    auto tk2 = parse_token("move E1E8", ' ');
    BOOST_REQUIRE(tk2);
    BOOST_REQUIRE_EQUAL((*tk2).first, string("move"));
    BOOST_REQUIRE_EQUAL((*tk2).second, string("E1E8"));
}
