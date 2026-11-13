#pragma once
namespace mde {

enum class Side { Bid, Ask };

struct L2Update {
    Side side;           // your existing enum
    std::int64_t price;  // ticks, from parse_ticks
    std::int64_t qty;    // ticks; 0 = remove the level
};
}