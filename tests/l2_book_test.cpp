#include <gtest/gtest.h>
#include <span>
#include <vector>
#include "mde/l2_book.hpp"

using mde::L2Book;
using mde::Level;
using mde::Side;
using Levels = std::vector<Level>;

namespace mde {
void PrintTo(const Level& l, std::ostream* os) { *os << '{' << l.price << ", " << l.qty << '}'; }
}

// Helper: copy a side into a vector so a whole side can be compared in one line.
static Levels toVec(std::span<const Level> s) { return {s.begin(), s.end()}; }

TEST(L2Book, EmptyBookHasNoBestPrices) {
    L2Book book(10);
    EXPECT_FALSE(book.bestBid().has_value());
    EXPECT_FALSE(book.bestAsk().has_value());
}

TEST(L2Book, BestBidAfterApplyBid) {
    L2Book book(10);
    book.apply(Side::Bid, 100, 1);
    EXPECT_EQ(book.bestBid(), (Level{100,1}));
}

TEST(L2Book, AddedBidsSorted) {
    L2Book book(10);
    book.apply(Side::Bid, 110, 1);
    book.apply(Side::Bid, 120, 1);
    book.apply(Side::Bid, 100, 1);
    EXPECT_EQ(toVec(book.bids()), (Levels{{120,1},{110,1},{100,1}}));
}

TEST(L2Book, AddedAsksSorted) {
    L2Book book(10);
    book.apply(Side::Ask, 110, 1);
    book.apply(Side::Ask, 120, 1);
    book.apply(Side::Ask, 100, 1);
    EXPECT_EQ(toVec(book.asks()), (Levels{{100,1},{110,1},{120,1}}));
}

TEST(L2Book, SamePriceQuantityReplaced) {
    L2Book book(10);
    book.apply(Side::Ask, 110, 1);
    book.apply(Side::Ask, 110, 5);  
    EXPECT_EQ(toVec(book.asks()), (Levels{{110,5}}));
}

TEST(L2Book, AskRemovedAfterZero) {
    L2Book book(10);
    book.apply(Side::Ask, 110, 1);
    book.apply(Side::Ask, 120, 1);
    book.apply(Side::Ask, 110, 0);
    EXPECT_EQ(toVec(book.asks()), (Levels{{120,1}}));
}

TEST(L2Book,ZeroAskChangesNothing) {
    L2Book book(10);
    book.apply(Side::Ask, 110, 1);
    book.apply(Side::Ask, 120, 1);
    book.apply(Side::Ask, 130, 0);
    EXPECT_EQ(toVec(book.asks()), (Levels{{110,1}, {120,1}}));
}

TEST(L2Book, ZeroBidChangesNothing) {
    L2Book book(3);
    book.apply(Side::Bid, 120, 1);
    book.apply(Side::Bid, 100, 1);
    book.apply(Side::Bid, 110, 0);
    EXPECT_EQ(toVec(book.bids()), (Levels{{120,1}, {100,1}}));
}

TEST(L2Book, DepthKeepsBestThree) {
    L2Book book(3);
    book.apply(Side::Bid, 110, 1);
    book.apply(Side::Bid, 120, 1);
    book.apply(Side::Bid, 130, 1);
    book.apply(Side::Bid, 140, 1);
    EXPECT_EQ(toVec(book.bids()), (Levels   {{140,1}, {130,1}, {120,1}}));
}

TEST(L2Book, WorseBidAtFullDepthIgnored) {
    L2Book book(3);
    book.apply(Side::Bid, 110, 1);
    book.apply(Side::Bid, 120, 1);
    book.apply(Side::Bid, 130, 1);
    book.apply(Side::Bid, 100, 1);
    EXPECT_EQ(toVec(book.bids()), (Levels{{130,1}, {120,1}, {110,1}}));
}

TEST(L2Book, BetterAskAtFullDepthOverrides) {
    L2Book book(3);
    book.apply(Side::Bid, 110, 1);
    book.apply(Side::Bid, 120, 1);
    book.apply(Side::Bid, 130, 1);
    book.apply(Side::Bid, 140, 1);
    EXPECT_EQ(toVec(book.bids()), (Levels{{140,1}, {130,1}, {120,1}}));
}

TEST(L2BookTest, ApplyL2UpdateAddsBid) {
    L2Book book(1);
    book.apply(mde::L2Update{mde::Side::Bid, 1000, 5});
    EXPECT_EQ(toVec(book.bids()), (Levels{{1000,5}}));
}