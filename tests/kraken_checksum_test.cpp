#include <gtest/gtest.h>
#include "mde/kraken_checksum.hpp"
#include "mde/l2_book.hpp"

namespace {
// Kraken docs, book checksum v2 example (ticks: price x10, qty x10^8)
mde::L2Book docs_example_book() {
    mde::L2Book book{10};   // ← adjust to your constructor
    const std::int64_t bids[10][2] = {
        {452835, 10000000}, {452834, 154582015}, {452821, 10000000}, {452810, 10000000},
        {452803, 154592586}, {452790, 7990000}, {452776, 3310103}, {452775, 30000000},
        {452773, 154602737}, {452766, 15445238}};
    const std::int64_t asks[10][2] = {
        {452852, 100000}, {452864, 154571953}, {452866, 154571109}, {452896, 154560911},
        {452902, 15890660}, {452918, 154553491}, {452947, 4454749}, {452961, 35380000},
        {452975, 9945542}, {452995, 18772827}};
    for (const auto& [p, q] : bids) book.apply(mde::L2Update{mde::Side::Bid, p, q});
    for (const auto& [p, q] : asks) book.apply(mde::L2Update{mde::Side::Ask, p, q});
    return book;
}

const std::string kDocsAsks =
    "45285210000045286415457195345286615457110945289615456091145290215890660"
    "452918154553491452947445474945296135380000452975994554245299518772827";
const std::string kDocsBids =
    "45283510000000452834154582015452821100000004528101000000045280315459258"
    "6452790799000045277633101034527753000000045277315460273745276615445238";



TEST(KrakenChecksum, DocsExampleString) {
    const auto book = docs_example_book();
    EXPECT_EQ(mde::kraken::checksum_string(book), kDocsAsks + kDocsBids);
}

TEST(KrakenChecksum, DocsExampleCrc) {
    const auto book = docs_example_book();
    EXPECT_EQ(mde::kraken::checksum(book), 3310070434u);
}
}  // namespace