#include <gtest/gtest.h>
#include"mde/kraken_book.hpp"
#include"kraken_fixtures.hpp"

TEST(KrakenBookSync, KrakenSyncSnapshotCorrect) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kSnapshot,1,8,msg);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    mde::kraken::BookSync sync{10}; 
    EXPECT_EQ(sync.on_message(msg), mde::kraken::BookSync::Result::Ok);
}

TEST(KrakenBookSync, KrakenSyncSnapshotWithUpdateCorrect) {
    mde::kraken::BookMessage snap,msg;
    mde::kraken::BookSync sync{10}; 
    auto parseStatus = parse_book_message(fixtures::kSnapshot,1,8,snap);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    ASSERT_EQ(sync.on_message(snap), mde::kraken::BookSync::Result::Ok);
    parseStatus = parse_book_message(fixtures::kUpdate,1,8,msg);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    EXPECT_EQ(sync.on_message(msg), mde::kraken::BookSync::Result::Ok);
}

TEST(KrakenBookSync, KrakenSyncRawUpdateFails) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kUpdate,1,8,msg);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    mde::kraken::BookSync sync{10}; 
    EXPECT_EQ(sync.on_message(msg), mde::kraken::BookSync::Result::UpdateBeforeSnapshot);
}

TEST(KrakenBookSync, KrakenSyncSnapshotWrongChecksumMismatch) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kSnapshot,1,8,msg);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    mde::kraken::BookSync sync{10}; 
    msg.checksum+=1;    
    EXPECT_EQ(sync.on_message(msg), mde::kraken::BookSync::Result::Mismatch);
}

TEST(KrakenBookSync, UpdateAfterMismatchRejected) {
    mde::kraken::BookMessage msg;
    auto parseStatus = parse_book_message(fixtures::kSnapshot,1,8,msg);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    mde::kraken::BookSync sync{10}; 
    msg.checksum+=1;    
    ASSERT_EQ(sync.on_message(msg), mde::kraken::BookSync::Result::Mismatch);
    parseStatus = parse_book_message(fixtures::kUpdate,1,8,msg);
    ASSERT_EQ( parseStatus, mde::kraken::ParseStatus::Ok );
    EXPECT_EQ(sync.on_message(msg), mde::kraken::BookSync::Result::UpdateBeforeSnapshot);
}