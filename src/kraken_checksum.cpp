#include <zlib.h>
#include <algorithm>
#include"mde/kraken_checksum.hpp"


namespace mde::kraken {
std::string checksum_string(const L2Book& book )
{
    std::string result;
    result.reserve(760); //19(int64 length)*2(price and qty)*2(asks and bids)*10 levels

    auto appendSide = [&result](std::span<const Level> sideSpan) {
        for (const auto& level : sideSpan.first(std::min<std::size_t>(10, sideSpan.size()))) 
        {
            result += std::to_string(level.price);
            result += std::to_string(level.qty);
        }
    };

    appendSide(book.asks());
    appendSide(book.bids());  

    return result;
}

std::uint32_t checksum(const L2Book& book) {
    const std::string s = checksum_string(book);
    return static_cast<std::uint32_t>(crc32(0, reinterpret_cast<const Bytef*>(s.data()), static_cast<uInt>(s.size())));
}

}