#include <gtest/gtest.h>
#include"mde/kraken_instrument.hpp"
#include"kraken_fixtures.hpp"

TEST(KrakenInstrument, FirstPairCorrect) {
    mde::kraken::PairInfo info;
    auto parseStatus = mde::kraken::parse_instrument(fixtures::kInstrumentSnapshot, "BTC/USD", info );
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    EXPECT_EQ( info.price_decimals, 1 );
    EXPECT_EQ( info.qty_decimals, 8 );
}

TEST(KrakenInstrument, SecondPairCorrect) {
    mde::kraken::PairInfo info;
    auto parseStatus = mde::kraken::parse_instrument(fixtures::kInstrumentSnapshot, "ETH/USD", info );
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    EXPECT_EQ( info.price_decimals, 2 );
    EXPECT_EQ( info.qty_decimals, 8 );
}

TEST(KrakenInstrument, PairNotInJsonError) {
    mde::kraken::PairInfo info;
    auto parseStatus = mde::kraken::parse_instrument(fixtures::kInstrumentSnapshot, "XRP/USD", info );
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Error );
}

TEST(KrakenInstrument, BookMessageIgnored) {
    mde::kraken::PairInfo info;
    auto parseStatus = mde::kraken::parse_instrument(fixtures::kSnapshot, "BTC/USD", info );
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ignored );
}

TEST(KrakenInstrument, UpdateIgnored) {
    mde::kraken::PairInfo info;
    auto parseStatus = mde::kraken::parse_instrument(fixtures::kInstrumentUpdate, "BTC/USD", info );
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ignored);
}

TEST(KrakenInstrument, NotJsonError) {
    mde::kraken::PairInfo info;
    auto parseStatus = mde::kraken::parse_instrument("{not json", "BTC/USD", info );
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Error );
}