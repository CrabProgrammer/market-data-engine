# market-data-engine

[![CI](https://github.com/CrabProgrammer/market-data-engine/actions/workflows/ci.yml/badge.svg)](https://github.com/CrabProgrammer/market-data-engine/actions/workflows/ci.yml)

## Goal

This engine receives live and historical market data, builds an order book from it and keeps the book up to date.
It can be used to monitor the current price, to estimate the cost of a large trade, or to study any moment of a past trading day.
The main purpose of the project is to learn how low-latency trading systems actually work, and possibly to grow it into a bigger system later.

## Architecture

Works now:

1. **Network**: connects to Kraken's WebSocket API, gets the price and quantity decimals for the pair from the instrument channel, then subscribes to the book and receives JSON messages: first a full snapshot, then updates.
2. **Kraken adapter**: turns each message into the engine's own `L2Update { side, price, qty }`, with prices as integer ticks. Kraken's format ends here.
3. **Book**: applies each update to the `L2Book` (quantity 0 removes a level).
4. **Checksum**: after each update, compares a CRC32 of the top 10 levels with Kraken's checksum; on a mismatch, resubscribes and rebuilds the book from a new snapshot.

Next (stage 2):

5. **SPSC queue**: passes updates from the network thread to a separate book thread without locks.
6. **Output**: mid price, spread and the cost of buying N, plus a latency histogram (p50 / p99 / p99.9) from "message received" to "book updated".

The tests use saved Kraken messages, so they run offline.

## Results

BTC/USD and ETH/USD, 50 seconds each: 7606 and 7371 updates verified by the checksum, 0 mismatches, 0 errors.
A test run with one checksum bit flipped on purpose showed 1 mismatch, 1 resubscribe, and the book was correct again after the new snapshot.

Details: [docs/RESULTS.md](docs/RESULTS.md). Design decisions and reasons: [docs/DECISIONS.md](docs/DECISIONS.md).

## Plan

| Stage | What works | Status |
|---|---|---|
| 0 | Build system, tests, CI (ASan/UBSan + TSan) | done |
| 1 | Live Kraken order book (any pair), verified by the exchange checksum, resync on mismatch | done |
| 2 | Network thread → lock-free SPSC queue → book thread, latency histograms | in progress |
| later | Nasdaq TotalView-ITCH 5.0 replay into order-by-order books; ITCH over UDP multicast | later |

## Build

Requires a C++20 compiler (GCC 14+), CMake 3.25+, Ninja and OpenSSL (`libssl-dev`).

```bash
cmake --preset asan && cmake --build --preset asan && ctest --preset asan
```

Presets: `asan` (AddressSanitizer + UBSan), `tsan` (ThreadSanitizer), `release` (benchmarks).

## Run

```bash
./build/asan/kraken_live            # BTC/USD
./build/asan/kraken_live ETH/USD    # any Kraken pair
MDE_TAMPER_AT=2000 ./build/asan/kraken_live   # flips one checksum bit at message 2000 to test the resync
```

The program runs for 50 seconds and prints the counters at the end.