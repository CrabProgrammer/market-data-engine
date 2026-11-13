#pragma once

#include <cstdint>
#include <optional>
#include <cmath>
#include <string_view>
#include <charconv>
namespace mde {
// Parses a decimal string ("65432.1") into a whole number of ticks,
// one tick = 10^-decimals. Returns std::nullopt on any malformed input.
std::optional<std::int64_t> parse_ticks(std::string_view text, int decimals);

}  // namespace mde