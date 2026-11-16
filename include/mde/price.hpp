#pragma once

#include <cstdint>
#include <optional>
#include <cmath>
#include <string_view>
#include <charconv>
namespace mde {
// Parses a decimal string into a whole number of tick, returns std::nullopt on a wrong input.
std::optional<std::int64_t> parse_ticks(std::string_view text, int decimals);

}  // namespace mde