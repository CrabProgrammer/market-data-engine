#include <simdjson.h>
#include "mde/kraken_instrument.hpp"

namespace mde::kraken {
ParseStatus parse_instrument(std::string_view json, std::string_view symbol, PairInfo& out) {

    using namespace simdjson;
    thread_local ondemand::parser parser;
    // simdjson needs a few padding bytes after the text; this copy adds them.
    const padded_string padded(json);

    ondemand::document doc;
    if (parser.iterate(padded).get(doc)) return ParseStatus::Error;

    std::string_view channel;
    auto channelError = doc["channel"].get_string().get(channel);
    if (channelError == NO_SUCH_FIELD) return ParseStatus::Ignored;
    if (channelError) return ParseStatus::Error;
    if (channel != "instrument") return ParseStatus::Ignored;

    std::string_view type;
    if (doc["type"].get_string().get(type)) return ParseStatus::Error;
    if (type != "snapshot") return ParseStatus::Ignored;  

    ondemand::object data;
    if (doc["data"].get_object().get(data)) return ParseStatus::Error;

    ondemand::array pairs; 
    if (data["pairs"].get_array().get(pairs)) return ParseStatus::Error;

    for(auto pair : pairs)
    {
        ondemand::object item; 
        if (pair.get_object().get(item)) return ParseStatus::Error;
        std::string_view currentSymbol;
        if(item["symbol"].get_string().get(currentSymbol)) return ParseStatus::Error;
        
        if(symbol != currentSymbol) continue;

        std::int32_t pd = 0, qd = 0;
        if (item["ws_display_price_precision"].get_int32().get(pd)) return ParseStatus::Error;   // not an int, or doesn't fit
        if (pd < 0 || pd > 18) return ParseStatus::Error;
        if (item["qty_precision"].get_int32().get(qd)) return ParseStatus::Error;   // not an int, or doesn't fit
        if (qd < 0 || qd > 18) return ParseStatus::Error;     
        out.price_decimals = pd;
        out.qty_decimals = qd;
        return ParseStatus::Ok;
    }
    return ParseStatus::Error;
}
}  // namespace mde::kraken