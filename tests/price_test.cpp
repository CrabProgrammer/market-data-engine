#include <gtest/gtest.h>
#include "mde/price.hpp"

using mde::parse_ticks;

TEST(ParseTick, OneDecimal) {
    auto result = parse_ticks("65432.1", 1);
    EXPECT_EQ(result, 654321);
}

TEST(ParseTick, NoDotPadsWithZeros) {
    auto result = parse_ticks("65432", 1);
    EXPECT_EQ(result, 654320);
}

TEST(ParseTick, TrailingZeroAccepted) {
    auto result = parse_ticks("65432.10", 1);
    EXPECT_EQ(result, 654321);
}

TEST(ParseTick, NonZeroBeyondPrecisionRejected) {
    auto result = parse_ticks("65432.15", 1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, QtyEightDecimals) {
    auto result = parse_ticks("0.00153210", 8);
    EXPECT_EQ(result, 153210);
}

TEST(ParseTick, QtyShortFractionPadded) {
    auto result = parse_ticks("0.0015321", 8);
    EXPECT_EQ(result, 153210);
}

TEST(ParseTick, EmptyRejected) {
    auto result = parse_ticks("", 1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, NonDigitRejected) {
    auto result = parse_ticks("12a.4", 1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, TwoDotsRejected) {
    auto result = parse_ticks("1.2.3", 1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, NegativeRejected) {
    auto result = parse_ticks("-5.0", 1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, DecimalsZeroRejectsNonZeroFraction) {
    auto result = parse_ticks("5.3", 0);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, NegativeDecimalsRejected) {
    auto result = parse_ticks("5.0", -1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, TwentyDigitsRejected) {
    auto result = parse_ticks("99999999999999999999", 0);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, ScaleOverflowRejected) {
    auto result = parse_ticks("922337203685477581", 1);
    EXPECT_EQ(result, std::nullopt);
}

TEST(ParseTick, MaxInt64Accepted) {
    auto result = parse_ticks("922337203685477580.7", 1);
    EXPECT_EQ(result, 9223372036854775807);
}

TEST(ParseTick, Int64OverflowRejected) {
    auto result = parse_ticks("922337203685477580.8", 1);
    EXPECT_EQ(result, std::nullopt);
}