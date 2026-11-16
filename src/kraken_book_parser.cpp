#include <simdjson.h>

#include "mde/kraken_book.hpp"
#include "mde/price.hpp"

namespace mde::kraken {
namespace {

std::string_view raw_text(simdjson::ondemand::value v) {
    std::string_view t = v.raw_json_token();
    while (!t.empty() && (t.back() == ' ' || t.back() == '\n' || t.back() == '\r' ||
                          t.back() == '\t')) {
        t.remove_suffix(1);
    }
    if (t.size() >= 2 && t.front() == '"' && t.back() == '"') {
        t = t.substr(1, t.size() - 2);
    }
    return t;
}

bool add_level(Side side, std::string_view price_text, std::string_view qty_text,
               int price_decimals, int qty_decimals, BookMessage& out) {

    const auto& price = parse_ticks(price_text, price_decimals);
    const auto& qty = parse_ticks(qty_text, qty_decimals);
    if(price && qty)
    {
        out.updates.emplace_back(side, price.value(), qty.value());
        return true;
    }
    return false;
}

}  // namespace

ParseStatus parse_book_message(std::string_view json, int price_decimals, int qty_decimals,
                               BookMessage& out) {
    using namespace simdjson;
    // One parser per thread, reused: no allocation per message once it has grown.
    thread_local ondemand::parser parser;
    // simdjson needs a few padding bytes after the text; this copy adds them.
    // (M3 can receive straight into a padded buffer and skip the copy.)
    const padded_string padded(json);

    out.updates.clear();

    ondemand::document doc;
    if (parser.iterate(padded).get(doc)) return ParseStatus::Error;

    std::string_view channel;
    const error_code channel_error = doc["channel"].get_string().get(channel);
    if (channel_error == NO_SUCH_FIELD) return ParseStatus::Ignored;   // e.g. subscribe ack
    if (channel_error) return ParseStatus::Error;                      // broken JSON
    if (channel != "book") return ParseStatus::Ignored;                // heartbeat, status...

    std::string_view type;
    if (doc["type"].get_string().get(type)) return ParseStatus::Error;
    if (type == "snapshot")      out.type = BookMessage::Type::Snapshot;
    else if (type == "update")   out.type = BookMessage::Type::Update;
    else                         return ParseStatus::Error;

    ondemand::array data;
    if (doc["data"].get_array().get(data)) return ParseStatus::Error;

    bool seen_entry = false;
    for (auto entry_result : data) {
        if (seen_entry) return ParseStatus::Error;   // we subscribe to one symbol: one entry
        seen_entry = true;

        ondemand::object entry;
        if (entry_result.get_object().get(entry)) return ParseStatus::Error;

        // Fields are read in the order Kraken sends them: bids, asks, checksum.
        for (auto [key, side] : {std::pair{"bids", Side::Bid}, std::pair{"asks", Side::Ask}}) {
            ondemand::array levels;
            if (entry[key].get_array().get(levels)) return ParseStatus::Error;
            for (auto level_result : levels) {
                ondemand::object level;
                if (level_result.get_object().get(level)) return ParseStatus::Error;
                ondemand::value price, qty;
                if (level["price"].get(price)) return ParseStatus::Error;
                const std::string_view price_text = raw_text(price);
                if (level["qty"].get(qty)) return ParseStatus::Error;
                const std::string_view qty_text = raw_text(qty);
                if (!add_level(side, price_text, qty_text, price_decimals, qty_decimals, out)) {
                    return ParseStatus::Error;
                }
            }
        }

        std::uint64_t checksum = 0;
        if (entry["checksum"].get_uint64().get(checksum) || checksum > UINT32_MAX) {
            return ParseStatus::Error;
        }
        out.checksum = static_cast<std::uint32_t>(checksum);
    }
    if (!seen_entry) return ParseStatus::Error;

    return ParseStatus::Ok;
}

}  // namespace mde::kraken