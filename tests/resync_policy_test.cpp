#include <gtest/gtest.h>
#include "mde/resync_policy.hpp"

using mde::kraken::ResyncPolicy;
using mde::kraken::ResyncAction;
using Type   = mde::kraken::BookMessage::Type;
using Result = mde::kraken::BookSync::Result;

TEST(ResyncPolicy, StartsWaiting) {
    ResyncPolicy p;
    EXPECT_EQ(p.resyncs(),0);
    EXPECT_EQ(p.waiting_for_snapshot(),true);
}

TEST(ResyncPolicy, UpdateBeforeSnapshotReturnsNone) {
    ResyncPolicy p;
    auto syncResult = p.on_sync(Type::Update, Result::UpdateBeforeSnapshot);
    EXPECT_EQ(syncResult, ResyncAction::None);
    EXPECT_EQ(p.resyncs(),0);
    EXPECT_EQ(p.waiting_for_snapshot(),true);
}

TEST(ResyncPolicy, NotWaitingAfterSnapshot) {
    ResyncPolicy p;
    auto syncResult = p.on_sync(Type::Snapshot, Result::Ok);
    EXPECT_EQ(syncResult, ResyncAction::None);
    EXPECT_EQ(p.waiting_for_snapshot(),false);
}
TEST(ResyncPolicy, ResubscribeAfterSnapshotMismatch) {
    ResyncPolicy p;
    auto syncResult = p.on_sync(Type::Snapshot, Result::Mismatch);
    EXPECT_EQ(syncResult, ResyncAction::Resubscribe);
    EXPECT_EQ(p.resyncs(),1);
    EXPECT_EQ(p.waiting_for_snapshot(),true);
}

TEST(ResyncPolicy, NoLoopAfterUpdates) {
    ResyncPolicy p;
    
    EXPECT_EQ(p.on_sync(Type::Snapshot, Result::Ok), ResyncAction::None);
    auto syncResult = p.on_sync(Type::Update, Result::Mismatch);
    EXPECT_EQ(syncResult, ResyncAction::Resubscribe);
    EXPECT_EQ(p.on_sync(Type::Update, Result::UpdateBeforeSnapshot),ResyncAction::None);
    EXPECT_EQ(p.on_sync(Type::Update, Result::UpdateBeforeSnapshot),ResyncAction::None);
    EXPECT_EQ(p.on_sync(Type::Update, Result::UpdateBeforeSnapshot),ResyncAction::None);
    EXPECT_EQ(p.resyncs(),1);
    EXPECT_EQ(p.waiting_for_snapshot(),true);
}

TEST(ResyncPolicy, ResubscribeAfterParseError) {
    ResyncPolicy p;
    EXPECT_EQ(p.on_parse_error(), ResyncAction::None);
    EXPECT_EQ(p.on_sync(Type::Snapshot, Result::Ok), ResyncAction::None);
    EXPECT_EQ(p.on_parse_error(), ResyncAction::Resubscribe);
    EXPECT_EQ(p.resyncs(), 1);
}

TEST(ResyncPolicy, ResubscribeAfterUpdateMismatch) {
    ResyncPolicy p;
    p.on_sync(Type::Snapshot, Result::Ok);
    EXPECT_EQ(p.on_sync(Type::Update, Result::Mismatch), ResyncAction::Resubscribe);
    EXPECT_EQ(p.resyncs(),1);
    EXPECT_EQ(p.waiting_for_snapshot(),true);
}