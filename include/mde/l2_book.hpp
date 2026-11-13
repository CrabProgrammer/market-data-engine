#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "l2_update.hpp"

namespace mde {

using Price = std::int64_t;   // ticks
using Qty   = std::int64_t;   // lots

struct Level {
    Price price;
    Qty qty;
    friend bool operator==(const Level&, const Level&) = default;
};

class L2Book {
public:
    explicit L2Book(std::size_t depth);
    void apply(Side side, Price price, Qty qty);   // qty is absolute; 0 = remove
    void apply(const L2Update& update);   
    std::optional<Level> bestBid() const;
    std::optional<Level> bestAsk() const;
    std::span<const Level> bids() const;           // best first
    std::span<const Level> asks() const;           // best first

private:
    std::size_t depth_;
    std::vector<Level> bids_;   // high → low
    std::vector<Level> asks_;   // low → high
};

}  // namespace mde