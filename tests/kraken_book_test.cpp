#include <gtest/gtest.h>
#include"mde/kraken_book.hpp"
#include"kraken_fixtures.hpp"

TEST(KrakenBook, KrakenSnapshotCorrect) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kSnapshot,1,8,msg);
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    EXPECT_EQ(msg.updates.size(), 20u);
    EXPECT_EQ(msg.checksum, 3310070434u);
    EXPECT_EQ(msg.updates.front(), (mde::L2Update{mde::Side::Bid, 452835, 10000000}));
}


TEST(KrakenBook, KrakenSnapshotQuotedCorrect) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kSnapshotQuoted,1,8,msg);
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    EXPECT_EQ(msg.updates.size(), 20u);
    EXPECT_EQ(msg.checksum, 3310070434u);
    EXPECT_EQ(msg.updates.front(), (mde::L2Update{mde::Side::Bid, 452835, 10000000}));
}

TEST(KrakenBook, KrakenHeartbeatIgnoredCorrect) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kHeartbeat,1,8,msg);
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Ignored );
}

TEST(KrakenBook, KrakenBadQtyError) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kUpdateBadQty,1,8,msg);
    EXPECT_EQ( parseStatus, mde::kraken::ParseStatus::Error);
}