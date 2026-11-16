#pragma once
#include <cstdint>
namespace mde {

enum class Side { Bid, Ask };

struct L2Update {
    Side side;
    std::int64_t price;  // ticks, from parse_ticks
    std::int64_t qty;    // ticks; 0 = remove the level
    friend bool operator==(const L2Update&, const L2Update&) = default;
};
}