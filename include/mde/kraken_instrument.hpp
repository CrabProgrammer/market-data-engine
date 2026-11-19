#pragma once
#include <string_view>
#include "mde/kraken_book.hpp"

namespace mde::kraken {

struct PairInfo {
    int price_decimals = 0;   // from ws_display_price_precision
    int qty_decimals   = 0;   // from qty_precision
};

ParseStatus parse_instrument(std::string_view json, std::string_view symbol, PairInfo& out);

}  // namespace mde::kraken