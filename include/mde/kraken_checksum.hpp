#pragma once

#include <cstdint>
#include <string>

#include "mde/l2_book.hpp"

namespace mde::kraken {

std::string   checksum_string(const L2Book& book);
std::uint32_t checksum(const L2Book& book);

}
