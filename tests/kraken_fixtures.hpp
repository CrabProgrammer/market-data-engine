#pragma once
#include <string_view>

namespace fixtures {

inline constexpr std::string_view kSnapshot = R"json({"channel":"book","type":"snapshot","data":[{"symbol":"BTC/USD","bids":[{"price":45283.5,"qty":0.10000000},{"price":45283.4,"qty":1.54582015},{"price":45282.1,"qty":0.10000000},{"price":45281.0,"qty":0.10000000},{"price":45280.3,"qty":1.54592586},{"price":45279.0,"qty":0.07990000},{"price":45277.6,"qty":0.03310103},{"price":45277.5,"qty":0.30000000},{"price":45277.3,"qty":1.54602737},{"price":45276.6,"qty":0.15445238}],"asks":[{"price":45285.2,"qty":0.00100000},{"price":45286.4,"qty":1.54571953},{"price":45286.6,"qty":1.54571109},{"price":45289.6,"qty":1.54560911},{"price":45290.2,"qty":0.15890660},{"price":45291.8,"qty":1.54553491},{"price":45294.7,"qty":0.04454749},{"price":45296.1,"qty":0.35380000},{"price":45297.5,"qty":0.09945542},{"price":45299.5,"qty":0.18772827}],"checksum":3310070434}]})json";

inline constexpr std::string_view kSnapshotQuoted = R"json({"channel":"book","type":"snapshot","data":[{"symbol":"BTC/USD","bids":[{"price":"45283.5","qty":"0.10000000"},{"price":"45283.4","qty":"1.54582015"},{"price":"45282.1","qty":"0.10000000"},{"price":"45281.0","qty":"0.10000000"},{"price":"45280.3","qty":"1.54592586"},{"price":"45279.0","qty":"0.07990000"},{"price":"45277.6","qty":"0.03310103"},{"price":"45277.5","qty":"0.30000000"},{"price":"45277.3","qty":"1.54602737"},{"price":"45276.6","qty":"0.15445238"}],"asks":[{"price":"45285.2","qty":"0.00100000"},{"price":"45286.4","qty":"1.54571953"},{"price":"45286.6","qty":"1.54571109"},{"price":"45289.6","qty":"1.54560911"},{"price":"45290.2","qty":"0.15890660"},{"price":"45291.8","qty":"1.54553491"},{"price":"45294.7","qty":"0.04454749"},{"price":"45296.1","qty":"0.35380000"},{"price":"45297.5","qty":"0.09945542"},{"price":"45299.5","qty":"0.18772827"}],"checksum":3310070434}]})json";

inline constexpr std::string_view kUpdate = R"json({"channel":"book","type":"update","data":[{"symbol":"BTC/USD","bids":[{"price":45283.4,"qty":0},{"price":45276.0,"qty":0.25000000}],"asks":[{"price":45285.2,"qty":0.50000000}],"checksum":3563522991,"timestamp":"2026-11-16T10:00:00.000000Z"}]})json";

inline constexpr std::string_view kUpdateBadQty =
    R"json({"channel":"book","type":"update","data":[{"symbol":"BTC/USD","bids":[{"price":45283.4,"qty":0.1x}],"asks":[],"checksum":1}]})json";

inline constexpr std::string_view kHeartbeat = R"json({"channel":"heartbeat"})json";

inline constexpr std::string_view kSubscribeAck =
    R"json({"method":"subscribe","result":{"channel":"book","depth":10,"snapshot":true,"symbol":"BTC/USD"},"success":true,"time_in":"2026-11-16T10:00:00.000000Z","time_out":"2026-11-16T10:00:00.000100Z"})json";

}  // namespace fixtures