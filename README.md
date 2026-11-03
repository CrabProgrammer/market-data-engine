# market-data-engine

[![CI](https://github.com/CrabProgrammer/market-data-engine/actions/workflows/ci.yml/badge.svg)](https://github.com/CrabProgrammer/market-data-engine/actions/workflows/ci.yml)

## Goal

This engine receives live and historical market data, builds an order book from it and keeps the book up to date.
It can be used to monitor the current price, to estimate the cost of a large trade, or to study any moment of a past trading day.
The main purpose of the project is to learn how low-latency trading systems actually work, and possibly to grow it into a bigger system later.

## Architecture (stages 1–2)

1. **Network thread**: connects to Kraken's WebSocket API, subscribes to the BTC/USD book and receives JSON messages: first a full snapshot, then updates.
2. **Kraken adapter**: turns each message into the engine's own `L2Update { side, price, qty }`, with prices as integer ticks. Kraken's format ends here.
3. **SPSC queue**: passes updates from the network thread to the book thread without locks.
4. **Book thread**: applies each update to the `L2Book` (quantity 0 removes a level).
5. **Checksum**: after each update, compares a CRC32 of the top 10 levels with Kraken's checksum; on a mismatch, resubscribes and rebuilds the book from a new snapshot.
6. **Output**: mid price, spread, imbalance and the cost of buying N BTC, plus a latency histogram (p50 / p99 / p99.9) from "message received" to "book updated".

Raw messages can also be recorded to a file and replayed offline; the tests use this.

## Plan

| Stage | What works | Status |
|---|---|---|
| 0 | Build system, tests, CI (ASan/UBSan + TSan) | done |
| 1 | Live Kraken BTC/USD order book, verified by the exchange checksum | in progress |
| 2 | Network thread → lock-free SPSC queue → book thread, latency histograms | planned |
| 3 | Full-day Nasdaq TotalView-ITCH 5.0 replay into order-by-order books | planned |
| 4 | ITCH over UDP multicast (MoldUDP64) with gap detection | planned |

## Build

Requires a C++20 compiler (GCC 14+), CMake 3.25+ and Ninja.

```bash
cmake --preset asan && cmake --build --preset asan && ctest --preset asan
```

Presets: `asan` (AddressSanitizer + UBSan), `tsan` (ThreadSanitizer), `release` (benchmarks).
