#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

#include "mde/l2_book.hpp"
#include "mde/l2_update.hpp"

namespace mde::kraken {

struct BookMessage {
    enum class Type { Snapshot, Update };
    Type type = Type::Update;
    std::vector<L2Update> updates;   // bids first, then asks
    std::uint32_t checksum = 0;
};

enum class ParseStatus { Ok, Ignored, Error };

//JSON to BookMessage 
ParseStatus parse_book_message(std::string_view json, int price_decimals, int qty_decimals,
                               BookMessage& out);


class BookSync {
public:
    enum class Result { Ok, Mismatch, UpdateBeforeSnapshot };

    explicit BookSync(std::size_t depth);

    // Process Snapshot/Update, then compare checksum
    Result on_message(const BookMessage& msg);

    const L2Book& book() const;

private:
    L2Book book_;
    bool have_snapshot_ = false;
};

}  // namespace mde::kraken